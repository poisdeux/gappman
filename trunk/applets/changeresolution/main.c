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
 * \todo add support for portrait orientation
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
#include <gm_generic.h>
#include <gm_layout.h>
#include <gm_connect.h>
#include <gm_changeresolution.h>

static int WINDOWED = 0;
static GtkWidget *mainwin;
static int fontsize;
static struct menu *programs = NULL;
static int dialog_width;
static int dialog_height;

static void usage()
{
	char *conffile = SYSCONFDIR "/conf.xml";
	printf
		("usage: changeresolution [--help] [--screenwidth <WIDTHINPIXELS>] [--screenheight <HEIGHTINPIXELS>] [--gmconffile <FILENAME>] [--gtkrc <GTKRCFILENAME>] [--windowed]\n");
	printf("\n");
	printf("--help:\t\tshows this help text\n");
	printf
		("--screenwidth <WIDTHINPIXELS>:\t\twidth of the main (gappman) window (default: screen width / 3)\n");
	printf
		("--screenheight <HEIGHTINPIXELS:\t\theight of the main (gappman) window (default: screen height / 3)\n");
	printf
		("--gmconffile <FILENAME>:\t\t configuration file specifying the program and actions (default: %s)\n",
		 conffile);
	printf
		("--gtkrc <GTKRCFILENAME>:\t\t gtk configuration file which can be used for themeing\n");
	printf("--windowed:\t\t creates a border around the window\n");

	exit(1);
}

/**
* \brief callback function to quit the program 
* \param *widget pointer to widget to destroy
* \param *event gdkevent that triggered calling this function
*/
static void destroy(GtkWidget * widget, GdkEvent * event)
{
	if (((GdkEventKey *) event)->keyval == 32
		|| ((GdkEventButton *) event)->button == 1)
	{
		gm_res_free();
		gtk_main_quit();
	}
}

static void revert_to_old_res(GtkWidget * widget, GdkEvent * event,
							  XRRScreenSize * size)
{
	// Check if spacebar or mousebutton is pressed
	if (((GdkEventKey *) event)->keyval == 32
		|| ((GdkEventButton *) event)->button == 1)
	{
		if (gm_res_changeresolution(size->width, size->height) ==
			GM_SIZE_NOT_AVAILABLE)
		{
			gm_show_error_dialog("Could not change resolution", mainwin, NULL);
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
										struct menu_element * elt)
{
	XRRScreenSize current_size;
	// Check if spacebar or mousebutton is pressed
	if (((GdkEventKey *) event)->keyval == 32
		|| ((GdkEventButton *) event)->button == 1)
	{
		if (gm_res_get_current_size(&current_size) == GM_SUCCES)
		{
			gm_set_default_resolution_for_program(2103, "localhost", elt->name,
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
	GtkWidget *button;
	GtkWidget *chooseprogramwin;
	GtkWidget *vbox;
	int button_height;
	int i;

	// only show menu if spacebar or mousebutton were pressed
	if (((GdkEventKey *) event)->keyval != 32
		&& ((GdkEventButton *) event)->button != 1)
		return;

	chooseprogramwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	gtk_window_set_transient_for(GTK_WINDOW(chooseprogramwin),
								 GTK_WINDOW(mainwin));
	gtk_window_set_position(GTK_WINDOW(chooseprogramwin),
							GTK_WIN_POS_CENTER_ON_PARENT);

	gtk_widget_grab_focus(chooseprogramwin);

	// Remove border
	gtk_window_set_decorated(GTK_WINDOW(chooseprogramwin), FALSE);

	if (programs != NULL)
	{
		vbox = gtk_vbox_new(FALSE, 10);
		button_height = dialog_height / (programs->amount_of_elements);
		for(i = 0; i < programs->amount_of_elements; i++)
		{
			button =
				gm_create_button(&(programs->elts[i]), dialog_width, button_height,
								 set_default_res_for_program);
			gtk_container_add(GTK_CONTAINER(vbox), button);
		}
		button =
			gm_create_label_button("Done", (void *)gm_destroy_widget,
								   chooseprogramwin);
		gtk_container_add(GTK_CONTAINER(vbox), button);
		gtk_container_add(GTK_CONTAINER(chooseprogramwin), vbox);
		gtk_widget_show_all(chooseprogramwin);
	}
	else
	{
		gm_show_error_dialog
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
	if (((GdkEventKey *) event)->keyval != 32
		&& ((GdkEventButton *) event)->button != 1)
		return;

	if (gm_res_get_current_size(&oldsize) != GM_SUCCES)
	{
		g_warning("Could not get current screen resolution");
	}

	if (gm_res_changeresolution(size[0].width, size[0].height) != GM_SUCCES)
	{
		gm_show_error_dialog("Could not change screen resolution", NULL, NULL);
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
		gm_create_label_button("Set resolution as default for program",
							   (void *)make_default_for_program, NULL);
	gtk_container_add(GTK_CONTAINER(vbox), button);

	button =
		gm_create_label_button("Keep resolution",
							   G_CALLBACK(gm_destroy_widget), confirmwin);
	gtk_container_add(GTK_CONTAINER(vbox), button);


	button =
		gm_create_label_button("Cancel", (void *)revert_to_old_res, &oldsize);
	(void)g_signal_connect(G_OBJECT(button), "key_release_event",
						   G_CALLBACK(gm_destroy_widget), confirmwin);
	(void)g_signal_connect(G_OBJECT(button), "button_release_event",
						   G_CALLBACK(gm_destroy_widget), confirmwin);
	gtk_container_add(GTK_CONTAINER(vbox), button);

	gtk_container_add(GTK_CONTAINER(confirmwin), vbox);

	gtk_widget_show_all(confirmwin);

	// We add a timer to return to the original resolution
	// after 10 seconds
	// g_timeout_add_seconds (10, )
}

static GtkWidget *show_current_resolution(int width)
{
	GtkWidget *hbox, *label;
	gchar *markup;
	XRRScreenSize  size;

	if ( gm_res_get_current_size(&size) != GM_SUCCES )
	{
		return NULL;
	}

	hbox = gtk_hbox_new(FALSE, 10);

	label = gtk_label_new("");

	markup =
		g_markup_printf_escaped("<span size=\"%d\">Current: %dx%d</span>", fontsize,
								size.width, size.height);
	gtk_label_set_markup(GTK_LABEL(label), markup);
	g_free(markup);
	gtk_container_add(GTK_CONTAINER(hbox), label);
	gtk_widget_show(label);

	return hbox;
}

static GtkWidget *createrow(XRRScreenSize * size, int width)
{
	GtkWidget *button, *hbox, *label;
	gchar *markup;

	hbox = gtk_hbox_new(FALSE, 10);

	button = gm_create_empty_button((void *)changeresolution, size);

	label = gtk_label_new("");

	markup =
		g_markup_printf_escaped("<span size=\"%d\">%dx%d</span>", fontsize,
								size->width, size->height);
	gtk_label_set_markup(GTK_LABEL(label), markup);
	g_free(markup);
	gtk_container_add(GTK_CONTAINER(button), label);
	gtk_widget_show(label);
	gtk_container_add(GTK_CONTAINER(hbox), button);
	gtk_widget_show(button);
	// gtk_container_add(GTK_CONTAINER(hbox), alignment);
	// gtk_widget_show(alignment);

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

	int c;
	int nsize;
	int ret_value;
	int i;
	XRRScreenSize *sizes;
	char *conffile = SYSCONFDIR "/conf.xml";
	gchar *gappman_confpath;

	gtk_init(&argc, &argv);
	screen = gdk_screen_get_default();
	dialog_width = gdk_screen_get_width(screen) / 3;
	dialog_height = gdk_screen_get_height(screen) / 3;

	mainwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	while (TRUE)
	{
		int option_index = 0;
		static struct option long_options[] = {
			{"width", 1, 0, 'w'},
			{"height", 1, 0, 'h'},
			{"conffile", 1, 0, 'c'},
			{"help", 0, 0, 'i'},
			{"gtkrc", 1, 0, 'r'},
			{"windowed", 0, 0, 'j'},
			{0, 0, 0, 0}
		};
		c = getopt_long(argc, argv, "w:h:c:r:ij", long_options, &option_index);
		if (c == -1)
			break;

		switch (c)
		{
		case 'w':
			dialog_width = atoi(optarg);
			break;
		case 'h':
			dialog_height = atoi(optarg);
			break;
		case 'c':
			conffile = (char *)optarg;
			break;
		case 'r':
			gtk_rc_parse(optarg);
			break;
		case 'j':
			WINDOWED = 1;
			break;
		default:
			usage();
			return 0;
		}
	}

	gtk_window_set_position(GTK_WINDOW(mainwin), GTK_WIN_POS_CENTER);
	// Remove border
	if (WINDOWED == 0)
	{
		gtk_window_set_decorated(GTK_WINDOW(mainwin), FALSE);
	}
	else
	{
		gtk_window_set_decorated(GTK_WINDOW(mainwin), TRUE);
		(void)g_signal_connect(G_OBJECT(mainwin), "delete_event",
							   G_CALLBACK(destroy), sizes);
		(void)g_signal_connect(G_OBJECT(mainwin), "destroy",
							   G_CALLBACK(destroy), sizes);
	}

	// get generic fontsize from gappman
	if (gm_get_fontsize_from_gappman(2103, "localhost", &fontsize) ==
		GM_SUCCES)
	{
		gm_set_fontsize(fontsize);
	}
	else
	{
		fontsize = gm_get_fontsize();
	}

	// get configuration file path from gappman
	if (gm_get_confpath_from_gappman(2103, "localhost", &gappman_confpath) ==
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
		gm_show_error_dialog("Changing screen resolution is not supported.",
							 NULL, (void *)destroy);
	}
	else
	{
		// Make window transparent
		// gtk_window_set_opacity (GTK_WINDOW (mainwin), 0.8);

		vbox = gtk_vbox_new(FALSE, 10);
	
		hbox = show_current_resolution(dialog_width);
		gtk_container_add(GTK_CONTAINER(vbox), hbox);
		gtk_widget_show(hbox);
		separator = gtk_hseparator_new();
		gtk_container_add(GTK_CONTAINER(vbox), separator);
		gtk_widget_show(separator);

		for (i = 0; i < nsize; i++)
		{
			hbox = createrow(&sizes[i], dialog_width);
			gtk_container_add(GTK_CONTAINER(vbox), hbox);
			gtk_widget_show(hbox);
			separator = gtk_hseparator_new();
			gtk_container_add(GTK_CONTAINER(vbox), separator);
			gtk_widget_show(separator);
		}
		hbox = gtk_hbox_new(FALSE, 10);
		// cancel button
		button = gm_create_label_button("Done", destroy, NULL);
		gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
		gtk_widget_show(button);

		gtk_container_add(GTK_CONTAINER(vbox), hbox);
		gtk_widget_show(hbox);
		gtk_container_add(GTK_CONTAINER(mainwin), vbox);
		gtk_widget_show(vbox);
		gtk_widget_show(mainwin);
	}


	gtk_main();

	return 0;
}
