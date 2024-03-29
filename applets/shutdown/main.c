/**
 * \file applets/shutdown/main.c
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
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>
#include <getopt.h>
#include <string.h>
#include <gm_parseconf.h>
#include <gm_layout.h>
#include <gm_network.h>

static int WINDOWED = 0;

static void usage()
{
	printf
		("usage: shutdown [--help] [--conffile <FILENAME>] [--gtkrc <GTKRCFILENAME>] [--windowed]\n");
	printf("\n");
	printf("--help:\t\tshows this help text\n");
	printf
		("--conffile <FILENAME>:\t\t configuration file specifying the program and actions (default: /etc/gappman/shutdown.xml)\n");
	printf
		("--gtkrc <GTKRCFILENAME>:\t\t gtk configuration file which can be used for themeing\n");
	printf("--windowed:\t\t creates a border around the window\n");

}

static void quit_program(GtkWidget * dummy, GdkEvent * event)
{
  // Only start program if spacebar or mousebutton is pressed
  if( gm_layout_check_key(event) )
  {
    gtk_main_quit();
  }
}

///< \todo check if startprogram can be replaced by a generic startprograms in a lib. BTW: Appmanager also uses a startprogram function.
static gboolean startprogram(GtkWidget * widget, gm_menu_element *elt)
{
	char **args;
	gint number_of_arguments;
	int i;
	__pid_t childpid;
	FILE *fp;

	/**
      Create argument list. First element should be the filename
      of the executable and last element needs to be NULL.
      see man exec for more details
    */
	number_of_arguments = gm_menu_element_get_amount_of_arguments(elt);
	args = (char **)malloc((number_of_arguments + 2) * sizeof(char *));
	args[0] = (char *)elt->exec;
	for (i = 0; i < number_of_arguments; i++)
	{
		args[i + 1] = elt->args[i];
	}
	args[i + 1] = NULL;

	fp = fopen((char *)elt->exec, "r");
	if (fp)
	{
		// Disable button
		gtk_widget_set_sensitive(GTK_WIDGET(widget), FALSE);

		(void)fclose(fp);

		childpid = fork();
		if (childpid == 0)
		{
			(void)execvp((char *)elt->exec, args);
			_exit(0);
		}
		else if (childpid < 0)
		{
			g_warning("Failed to fork!\n");
			return FALSE;
		}
	}
	else
	{
		g_warning("File: %s not found!\n", (char *)elt->exec);
		return FALSE;
	}

	free(args);
	return TRUE;
}


/**
* \brief function that starts any program as defined by the structure *elt.
* \param *widget pointer to the button widget which has the process_startprogram_event connected through a signal
* \param *event the GdkEvent that occured. Space key and left mousebutton are valid actions.
* \param *elt menu_element structure containing the filename and arguments of the program that should be started
*/
static void process_startprogram_event(GtkWidget * widget, GdkEvent * event,
									   gm_menu_element * elt)
{


	// Only start program if spacebar or mousebutton is pressed
	if (((GdkEventKey *) event)->keyval == 32
		|| ((GdkEventButton *) event)->button == 1)
	{
		(void)startprogram(widget, elt);
	}

}

/**
* \brief callback function to quit the program
* \param *widget pointer to widget to destroy
* \param data mandatory argument for callback function, may be NULL.
*/
static void destroy(GtkWidget * widget, gpointer data)
{
	gtk_main_quit();
}

GtkWidget *menu_create(gm_menu *menu)
{
  gint i;
  gint width;
  gint height;
  GtkWidget *button;
  GtkWidget *buttonbox;
	gm_menu_element *menu_element;

  gm_layout_calculate_sizes(menu);

  width = menu->widget_width;
  height = menu->widget_height;

  for(i = 0; i < menu->amount_of_elements; i++)
  {
		menu_element = gm_menu_get_menu_element(i, menu);
    button = gm_layout_create_button(menu_element, width, height, process_startprogram_event);
    gm_menu_element_set_widget(button, menu_element);
  }

  buttonbox = gm_layout_create_menu(menu);

  return buttonbox;
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
	GtkWidget *align;
	GtkWidget *mainwin;
	gm_menu *actions;
	const char *conffile = SYSCONFDIR "/shutdown.xml";
	int c;
#if defined(DEBUG)
  int width, height;
#endif

	gtk_init(&argc, &argv);

	while (TRUE)
	{
		int option_index = 0;
		static struct option long_options[] = {
			{"conffile", 1, 0, 'c'},
			{"help", 0, 0, 'i'},
			{"gtkrc", 1, 0, 'r'},
			{"windowed", 0, 0, 'j'},
			{0, 0, 0, 0}
		};
		c = getopt_long(argc, argv, "c:r:ij",
						long_options, &option_index);
		if (c == -1)
			break;

		switch (c)
		{
		case 'c':
			conffile = optarg;
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

	/** Load configuration elements */
	if (gm_load_conf(conffile) != 0)
	{
		g_error("Error could not load configuration: %s", conffile);
	}

	actions = gm_get_actions();

	screen = gdk_screen_get_default();

	mainwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	gtk_window_set_position(GTK_WINDOW(mainwin), GTK_WIN_POS_CENTER);

	// Make window transparent
	// gtk_window_set_opacity (GTK_WINDOW (mainwin), 0.8);

	// Remove border
	if (WINDOWED == 0)
	{
		gtk_window_set_decorated(GTK_WINDOW(mainwin), FALSE);
	}
	else
	{
		gtk_window_set_decorated(GTK_WINDOW(mainwin), TRUE);
		(void)g_signal_connect(G_OBJECT(mainwin), "delete_event",
							   G_CALLBACK(destroy), NULL);
		(void)g_signal_connect(G_OBJECT(mainwin), "destroy",
							   G_CALLBACK(destroy), NULL);
	}
	vbox = gtk_vbox_new(FALSE, 10);

#if defined(DEBUG)
  gm_network_get_window_geometry_from_gappman(2103, "localhost", &width, &height);
  gm_layout_set_window_geometry(width, height);
#endif

	if (actions != NULL)
	{
		align = menu_create(actions);
		gtk_container_add(GTK_CONTAINER(vbox), align);
		gtk_widget_show(align);
	}

	hbox = gtk_hbox_new(FALSE, 10);

	// cancel button
	button = gm_layout_create_label_button("Cancel", (void *)quit_program, NULL);
	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
	gtk_widget_show(button);

	gtk_container_add(GTK_CONTAINER(vbox), hbox);
	gtk_widget_show(hbox);

	gtk_container_add(GTK_CONTAINER(mainwin), vbox);
	gtk_widget_show(vbox);

	// make sure the widget grabs keyboard and mouse focus
	gtk_widget_grab_focus(mainwin);

	gtk_widget_show(mainwin);

	gtk_main();

	gm_menu_free(actions);

	return 0;
}
