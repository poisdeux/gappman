/**
 * \file applets/changeresolution/main.c
 *
 *
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 *
 */

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <glib/gprintf.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>
#include <getopt.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <gm_layout.h>
#include <gm_network.h>
#include <gm_changeresolution.h>
#include <gm_parseconf.h>

static GtkWidget *mainwin;
static gm_menu *programs = NULL;
static int window_width;
static int window_height;

static void usage()
{
	printf
		("usage: changeresolution [--help] [--screenwidth <WIDTHINPIXELS>] [--screenheight <HEIGHTINPIXELS>] [--gtkrc <GTKRCFILENAME>] [--windowed]\n");
	printf("\n");
	printf("--help:\t\tshows this help text\n");
	printf
		("--screenwidth <WIDTHINPIXELS>:\t\twidth of the main (gappman) window (default: screen width / 3)\n");
	printf
		("--screenheight <HEIGHTINPIXELS:\t\theight of the main (gappman) window (default: screen height / 3)\n");
	printf
		("--gtkrc <GTKRCFILENAME>:\t\t gtk configuration file which can be used for themeing\n");
	printf("--windowed:\t\t creates a border around the window\n");

	exit(1);
}

static void destroy_widget(GtkWidget *dummy, GdkEvent * event, GtkWidget *widget)
{
  // Only start program if spacebar or mousebutton is pressed
  if( gm_layout_check_key(event) )
  {
    gtk_widget_destroy(widget);
  }
}

/**
* \brief callback function to quit the program 
* \param *widget pointer to widget to destroy
* \param *event gdkevent that triggered calling this function
*/
static void quit_program(GtkWidget * widget, GdkEvent * event)
{
	if( gm_layout_check_key(event) )
	{
		gm_res_free();
		gtk_main_quit();
	}
}

static void revert_to_old_res(GtkWidget * widget, GdkEvent * event,
							  XRRScreenSize * size)
{
	// Check if spacebar or mousebutton is pressed
	if( gm_layout_check_key(event) )
	{
		if (gm_res_changeresolution(size->width, size->height) ==
			GM_SIZE_NOT_AVAILABLE)
		{
			gm_layout_show_error_dialog("Could not change resolution", mainwin, NULL);
		}
	}
}

/**
* \brief Sends the new resolution to be used as default for a program to gappman
* \param *widget that called this function (usually through a callback construction)
* \param *event event that triggered the widget
* \param *elt menu_element pointer to the program for which the resolution must be updated
*/
static void set_default_res_for_program(GtkWidget * widget, GdkEvent * event,
										gm_menu_element * elt)
{
	XRRScreenSize current_size;
	// Check if spacebar or mousebutton is pressed
	if( gm_layout_check_key(event) )
	{
		if (gm_res_get_current_size(&current_size) == GM_SUCCES)
		{
			gm_network_set_default_resolution_for_program(2103, "localhost", gm_menu_element_get_name(elt),
												  current_size.width,
												  current_size.height);
		}
		else
		{
			g_warning("Could not get current screen resolution");
		}
	}
}

/**
* \brief creates a popup dialog window that allows the user to set the new resolution as default for a program
* \param *widget that called this function (usually through a callback construction)
* \param *event event that triggered the widget
*/
static void make_default_for_program(GtkWidget * widget, GdkEvent * event)
{
	GtkWidget *chooseprogramwin;
	GtkWidget *buttonbox;
	GtkWidget *button;
	GtkWidget *vbox;
	int window_width, window_height;

	// only show menu if spacebar or mousebutton were pressed
	if( ! gm_layout_check_key(event) )
		return;

		if (programs != NULL)
	{
		chooseprogramwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);

		gtk_window_set_transient_for(GTK_WINDOW(chooseprogramwin),
								 GTK_WINDOW(mainwin));
		gtk_window_set_position(GTK_WINDOW(chooseprogramwin),
							GTK_WIN_POS_CENTER_ON_PARENT);

		gtk_widget_grab_focus(chooseprogramwin);

		// Remove border
		gtk_window_set_decorated(GTK_WINDOW(chooseprogramwin), FALSE);

		//we need to reduce dialog height to make room for the cancel button
		gm_layout_get_window_geometry(&window_width, &window_height);
		gm_layout_set_window_geometry(window_width, 0.9*window_height);

		buttonbox = gm_layout_create_menu(programs, &set_default_res_for_program);

		vbox = gtk_vbox_new(FALSE, 10);
		gtk_container_add(GTK_CONTAINER(vbox), buttonbox);

		button = gm_layout_create_label_button("Cancel", destroy_widget, chooseprogramwin);
		gtk_container_add(GTK_CONTAINER(vbox), button);

		gtk_container_add(GTK_CONTAINER(chooseprogramwin), vbox);
		gtk_widget_show_all(chooseprogramwin);
	}
	else
	{
		gm_layout_show_error_dialog
			("No programs found.\nPlease check configuration file.", NULL,
			 NULL);
	}
}

/**
* \brief creates a popup dialog window that allows the user confirm the new resolution
* \param *widget that called this function (usually through a callback construction)
* \param *event event that triggered the widget
* \param *size pointer to a XRRScreenSize struct that holds the new resolution
*/
static void changeresolution(GtkWidget * widget, GdkEvent * event,
							 XRRScreenSize * size)
{
	GtkWidget *button;
	GtkWidget *vbox;
	GtkWidget *confirmwin;
	static XRRScreenSize oldsize;

	// only show menu if spacebar or mousebutton were pressed
	if( ! gm_layout_check_key(event) )
		return;

	if (gm_res_get_current_size(&oldsize) != GM_SUCCES)
	{
		g_warning("Could not get current screen resolution");
	}

	if (gm_res_changeresolution(size[0].width, size[0].height) != GM_SUCCES)
	{
		gm_layout_show_error_dialog("Could not change screen resolution", NULL, NULL);
		return;
	}

	confirmwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	gtk_window_set_transient_for(GTK_WINDOW(confirmwin), GTK_WINDOW(mainwin));
	gtk_window_set_position(GTK_WINDOW(confirmwin),
							GTK_WIN_POS_CENTER_ON_PARENT);

	// Make window transparent
	// gtk_window_set_opacity (GTK_WINDOW (confirmwin), 0.8);

	// Remove border
	gtk_window_set_decorated(GTK_WINDOW(confirmwin), FALSE);

	vbox = gtk_vbox_new(TRUE, 10);

	button =
		gm_layout_create_label_button("Set resolution as default for program",
							   (void *)make_default_for_program, NULL);
	gtk_container_add(GTK_CONTAINER(vbox), button);

	button =
		gm_layout_create_label_button("Keep resolution",
							   G_CALLBACK(destroy_widget), confirmwin);
	gtk_container_add(GTK_CONTAINER(vbox), button);


	button =
		gm_layout_create_label_button("Cancel", (void *)revert_to_old_res, &oldsize);
	(void)g_signal_connect(G_OBJECT(button), "key_release_event",
						   G_CALLBACK(destroy_widget), confirmwin);
	(void)g_signal_connect(G_OBJECT(button), "button_release_event",
						   G_CALLBACK(destroy_widget), confirmwin);
	gtk_container_add(GTK_CONTAINER(vbox), button);

	gtk_container_add(GTK_CONTAINER(confirmwin), vbox);

	gtk_widget_show_all(confirmwin);

  ///< \todo Add timer to fallback to previous resolution when user does not respond
}

static GtkWidget *show_current_resolution()
{
	GtkWidget *hbox, *label;
	gchar *labeltext;
	XRRScreenSize  size;

	if ( gm_res_get_current_size(&size) != GM_SUCCES )
	{
		return NULL;
	}

	hbox = gtk_hbox_new(FALSE, 10);

	labeltext = g_strdup_printf("Current: %dx%d", size.width, size.height);
	label = gm_layout_create_label(labeltext);
	gtk_container_add(GTK_CONTAINER(hbox), label);
	gtk_widget_show(label);

	return hbox;
}

/**
* \brief main function setting up the UI
*/
int main(int argc, char **argv)
{
	GdkScreen *screen;
	GtkWidget *button;
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *separator;
	gint c;
	gint nsize;
	gint ret_value;
	gint i;
  gint fontsize;
	XRRScreenSize *sizes;
	gchar *gappman_confpath = SYSCONFDIR "/conf.xml";
	gchar *text;
	int window_width, window_height;

	gtk_init(&argc, &argv);
	screen = gdk_screen_get_default();
	window_width = gdk_screen_get_width(screen);
	window_height = gdk_screen_get_height(screen);

#if defined(DEBUG)
gm_network_get_window_geometry_from_gappman(2103, "localhost", &window_width, &window_height);
#endif

	gm_layout_set_window_geometry(window_width, window_height);

	mainwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	while (TRUE)
	{
		int option_index = 0;
		static struct option long_options[] = {
			{"help", 0, 0, 'i'},
			{"gtkrc", 1, 0, 'r'},
			{0, 0, 0, 0}
		};
		c = getopt_long(argc, argv, "h:r:", long_options, &option_index);
		if (c == -1)
			break;

		switch (c)
		{
		case 'r':
			gtk_rc_parse(optarg);
			break;
		default:
			usage();
			return 0;
		}
	}

	gtk_window_set_position(GTK_WINDOW(mainwin), GTK_WIN_POS_CENTER);
	// Remove border
	gtk_window_set_decorated(GTK_WINDOW(mainwin), FALSE);

	// get generic fontsize from gappman
	if (gm_network_get_fontsize_from_gappman(2103, "localhost", &fontsize) ==
		GM_SUCCES)
	{
		gm_layout_set_fontsize(fontsize);
	}

	// get configuration file path from gappman
	if (gm_network_get_confpath_from_gappman(2103, "localhost", &gappman_confpath) ==
		GM_SUCCES)
	{
		if (gm_load_conf(gappman_confpath) == GM_SUCCES)
		{
			programs = gm_get_programs();
		}
	}

	gm_res_init();

	ret_value = gm_res_getpossibleresolutions(&sizes, &nsize);
	if (ret_value != GM_SUCCES)
	{
		g_warning
			("Error could not get possible resolutions (error_type: %d)\n",
			 ret_value);
		gm_layout_show_error_dialog("Changing screen resolution is not supported.",
							 NULL, (void *)quit_program);
	}
	else
	{
		vbox = gtk_vbox_new(FALSE, 10);
	
		hbox = show_current_resolution();
		gtk_container_add(GTK_CONTAINER(vbox), hbox);
		gtk_widget_show(hbox);
		separator = gtk_hseparator_new();
		gtk_container_add(GTK_CONTAINER(vbox), separator);
		gtk_widget_show(separator);

		for (i = 0; i < nsize; i++)
		{
			text = g_strdup_printf("%dx%d",sizes[i].width, sizes[i].height);
			button = gm_layout_create_label_button(text, (void *)changeresolution, &sizes[i]);
			g_free(text);
			gtk_container_add(GTK_CONTAINER(vbox), button);
			separator = gtk_hseparator_new();
			gtk_container_add(GTK_CONTAINER(vbox), separator);
		}
		// cancel button
		button = gm_layout_create_label_button("Done", quit_program, NULL);
		gtk_container_add(GTK_CONTAINER(vbox), button);
		gtk_widget_show(button);

		gtk_container_add(GTK_CONTAINER(mainwin), vbox);
		gtk_widget_show_all(mainwin);
	}

	gtk_main();

	return 0;
}
