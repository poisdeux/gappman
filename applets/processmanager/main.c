/**
 * \file applets/processmanager/main.c
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
#include <gm_network.h>
#include <gm_parseconf.h>
#include <gm_layout.h>

#define WINDOW_PERCENTAGE 0.5

static int WINDOWED = 0;
static GtkWidget *mainwin;
//static char *color[6] = { "green", "green", "red", "orange", "blue", "yellow" };
//static char *statusarray[6] =
//	{ "running", "sleeping", "stopped", "waiting", "zombie", "paging" };
static int fontsize;
static gm_menu *programs;
static gm_menu *actions;
static gm_menu *menu;

enum pid_status 
{
RUNNING,
SLEEPING,
TRACED_OR_STOPPED,
UNINTERRUPTIBLE_DISK_SLEEP,
ZOMBIE,
PAGING,
NONE
};

static void usage()
{
	printf
		("usage: processmanager [--help] [--width <WIDTHINPIXELS>] [--height <HEIGHTINPIXELS>] [--gtkrc <GTKRCFILENAME>] [--windowed]\n");
	printf("\n");
	printf("--help:\t\tshows this help text\n");
	printf
		("--width <WIDTHINPIXELS>:\t\twidth of the main window (default: screen width / 9)\n");
	printf
		("--height <HEIGHTINPIXELS:\t\theight of the main window (default: screen height / 9)\n");
	printf
		("--gtkrc <GTKRCFILENAME>:\t\t gtk configuration file which can be used for themeing\n");
	printf("--windowed:\t\t creates a border around the window\n");

}

static void destroy_widget(GtkWidget * dummy, GdkEvent * event, GtkWidget * widget)
{
  if ( gm_layout_check_key(event) )
  {
    gtk_widget_destroy(widget);
  }
}

static void quit_program()
{
	gm_menu_free(programs);
	gm_menu_free(actions);
	gm_menu_free(menu);
  gtk_main_quit();
}

static void quit_program_callback(GtkWidget * dummy, GdkEvent * event, void *data)
{
	if(gm_layout_check_key(event))
	{
		quit_program();
	}
}

static int get_status(int pid)
{
	char *proc_string = malloc((strlen("/proc/stat") + 6) * sizeof(char));
	gchar *contents = NULL;
	gchar **contentssplit = NULL;
	gsize length;
	GError *gerror = NULL;
	gchar *status = NULL;
	enum pid_status ret_status;

	ret_status = NONE;
	g_sprintf(proc_string, "/proc/%d/stat", pid);
	if (g_file_get_contents(proc_string, &contents, &length, &gerror))
	{
		contentssplit = g_strsplit(contents, " ", 4);

		status = contentssplit[2];

		// RUNNING
		if (g_strcmp0("R", status) == 0)
		{
			ret_status = RUNNING;
		}
		else if (g_strcmp0("S", status) == 0)
		{
			ret_status = SLEEPING;
		}
		else if (g_strcmp0("T", status) == 0)
		{
			ret_status = TRACED_OR_STOPPED;
		}
		else if (g_strcmp0("D", status) == 0)
		{
			ret_status = UNINTERRUPTIBLE_DISK_SLEEP;
		}
		else if (g_strcmp0("Z", status) == 0)
		{
			ret_status = ZOMBIE;
		}
		else if (g_strcmp0("W", status) == 0)
		{
			ret_status = PAGING;
		}
		g_free(contents);
		g_strfreev(contentssplit);
	}

	free(proc_string);

	return ret_status;
}

static int kill_program(GtkWidget * widget, GdkEvent * event,
						gm_menu_element * elt)
{
	enum pid_status status;
	int wait_count;
	int pid;

	if ( ! gm_layout_check_key( event ) )
		return FALSE;

	// disable kill button
	gtk_widget_set_sensitive(widget, FALSE);

	pid = gm_menu_element_get_pid(elt);

	status = get_status(pid);
	switch (status)
	{
	case RUNNING:
		kill(pid, 15);
		break;;
	case SLEEPING:
		kill(pid, 15);
		break;;
	case TRACED_OR_STOPPED:
		kill(pid, 9);
		break;;
	case UNINTERRUPTIBLE_DISK_SLEEP:
		kill(pid, 9);
		break;;
	case ZOMBIE:
		// What to do with zombies? Kill parent?
		g_warning("Oh my god! It's a zombie. Who's your daddy!?\n");
		return FALSE;;
	case NONE:
		break;;
	default:
		g_warning("Huh? What should I do with status %d for proces %d\n",
				  status, pid);
		return FALSE;;
	}


	wait_count = 0;
	//check again to see if the kill was succesful quickly
  //to prevent a 1 second sleep
	while ( 1 )
	{
		status = get_status(pid);
		if( ( status == NONE ) || (wait_count > 10))
			break;

		sleep(1);
		wait_count++;
	}

	if ( status == NONE )
	{
		// remove program from main window
		gtk_widget_set_sensitive(elt->widget, FALSE);
		gtk_widget_destroy(g_object_get_data((GObject *) widget, "window"));
	}
	if (status != get_status(pid))
	{
		// Process changed status.
		// Let's try again
		(void)kill_program(widget, event, elt);
	}
	else
	{
		// make button active again so user may try again 
		gtk_widget_set_sensitive(widget, TRUE);
	}

	return TRUE;
}


/**
* \brief creates a popup dialog window that allows the user to stop a program
* \param *elt pointer to menu_element structure that contains the program to be stopped
*/
static void showprocessdialog(gm_menu_element * elt)
{
	GtkWidget *button, *buttonbox, *label;
	static GtkWidget *killdialogwin;
	gchar *msg;

	killdialogwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	gtk_window_set_transient_for(GTK_WINDOW(killdialogwin),
								 GTK_WINDOW(mainwin));
	gtk_window_set_position(GTK_WINDOW(killdialogwin),
							GTK_WIN_POS_CENTER_ON_PARENT);

	// Remove border
	gtk_window_set_decorated(GTK_WINDOW(killdialogwin), FALSE);

	buttonbox = gtk_hbutton_box_new();

	msg = g_strdup_printf("Stop %s", elt->name);
	button = gm_layout_create_label_button(msg, (void *)kill_program, elt);
	gtk_container_add(GTK_CONTAINER(buttonbox), button);
	gtk_widget_show(button);
	g_free(msg);

	// Needed so we can destroy the dialog when kill was succesful
	g_object_set_data((GObject *) button, "window", killdialogwin);

	button =
		gm_layout_create_label_button("Cancel", (void *)destroy_widget,
							   killdialogwin);
	gtk_container_add(GTK_CONTAINER(buttonbox), button);
	gtk_widget_show(button);

	gtk_container_add(GTK_CONTAINER(killdialogwin), buttonbox);
	gtk_widget_show(buttonbox);
	gtk_widget_show(killdialogwin);
	gtk_widget_grab_focus(button);
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
	if ( gm_layout_check_key(event) )
	{
		gtk_widget_set_sensitive(GTK_WIDGET(widget), FALSE);
		showprocessdialog(elt);
		gtk_widget_set_sensitive(GTK_WIDGET(widget), TRUE);
	}
}

GtkWidget *create_menu(struct proceslist *started_procs)
{
	gm_menu_element *menu_elt;
	gm_menu_element *new_elt;
	struct proceslist *started_procs_tmp;
	GtkWidget *vbox;
	GtkWidget *box;
	GtkWidget *button;
	gint no_progsacts_found;
	gint mypid;
	gint width, height;
	
	programs = gm_get_programs();
	actions = gm_get_actions();
	menu = gm_menu_create();

	started_procs_tmp = started_procs;
	mypid = getpid();
	no_progsacts_found = 1;

	gm_menu_set_width(PERCENTAGE, 80, menu);
  gm_menu_set_height(PERCENTAGE, 80, menu);
  gm_menu_set_max_elts_in_single_box(12, menu);

	gm_layout_calculate_sizes(menu);
	width = menu->widget_width;
	height = menu->widget_height;

	while (started_procs != NULL)
	{
		if( started_procs->pid == mypid )
		{
			started_procs = started_procs->prev;
			continue;
		}

		menu_elt = gm_menu_search_elt_by_name(started_procs->name, programs);
		if( menu_elt != NULL )
		{
			no_progsacts_found = 0;
			new_elt = gm_menu_element_create();
			gm_menu_element_set_pid(started_procs->pid, new_elt);			
			gm_menu_element_set_name(gm_menu_element_get_name(menu_elt), new_elt);
			gm_menu_element_set_print_label(gm_menu_element_get_print_label(menu_elt), new_elt);
			gm_menu_element_set_logo(gm_menu_element_get_logo(menu_elt), new_elt);
			button = gm_layout_create_button(new_elt, width, height, process_startprogram_event);
			gm_menu_element_set_widget(button, new_elt);
			gm_menu_add_menu_element(new_elt, menu);
		}

		menu_elt = gm_menu_search_elt_by_name(started_procs->name, actions);
		if( menu_elt != NULL )
		{
			no_progsacts_found = 0;
			new_elt = gm_menu_element_create();
			gm_menu_element_set_pid(started_procs->pid, new_elt);
			button = gm_layout_create_button(menu_elt, width, height, process_startprogram_event);
			gm_menu_element_set_widget(button, new_elt);
			gm_menu_add_menu_element(new_elt, menu);
		}

		started_procs = started_procs->prev;
	}

	gm_network_free_proceslist(started_procs);

	if ( no_progsacts_found )
	{
		gm_layout_show_error_dialog("No programs or actions started.", NULL, quit_program_callback);
		return NULL;
	}

	vbox = gtk_vbox_new(FALSE, 10);
	
	gm_layout_calculate_sizes(menu);
	box = gm_layout_create_menu(menu);
	gtk_container_add(GTK_CONTAINER(vbox), box);
	gtk_widget_show(box);

	button =
		gm_layout_create_label_button("Cancel", (void *)quit_program_callback,
							   NULL);
	gtk_container_add(GTK_CONTAINER(vbox), button);
	gtk_widget_show(button);
	
 	return vbox;
}

/**
* \brief main function setting up the UI
*/
int main(int argc, char **argv)
{
	GdkScreen *screen;
	GtkWidget *vbox;
	gchar *gappman_confpath;
	gchar *msg;
	int dialog_width;
	int dialog_height;
	int program_width;
	int no_progsacts_found;	//< used to determine if any progs were started by gappman. If this program (processmanager) was started using gappman.
	int row_height;
	GmReturnCode status;
	int c;
	struct proceslist *started_procs;

	gtk_init(&argc, &argv);
	screen = gdk_screen_get_default();
	dialog_width = gdk_screen_get_width(screen) * WINDOW_PERCENTAGE;
	dialog_height = gdk_screen_get_height(screen) * WINDOW_PERCENTAGE;
	mainwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);

#if defined(DEBUG)
  gm_network_get_window_geometry_from_gappman(2103, "localhost", &dialog_width, &dialog_height);
	dialog_width = dialog_width * WINDOW_PERCENTAGE;
	dialog_height = dialog_height * WINDOW_PERCENTAGE;
  gm_layout_set_window_geometry(dialog_width, dialog_height);
#endif



	while (TRUE)
	{
		int option_index = 0;
		static struct option long_options[] = {
			{"width", 1, 0, 'w'},
			{"height", 1, 0, 'h'},
			{"help", 0, 0, 'i'},
			{"gtkrc", 1, 0, 'r'},
			{"windowed", 0, 0, 'j'},
			{0, 0, 0, 0}
		};
		c = getopt_long(argc, argv, "w:h:d:r:ij",
						long_options, &option_index);
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
		g_signal_connect(G_OBJECT(mainwin), "delete_event",
						 G_CALLBACK(quit_program), NULL);
		g_signal_connect(G_OBJECT(mainwin), "destroy",
						 G_CALLBACK(quit_program), NULL);
	}

/*
	status = gm_network_get_fontsize_from_gappman(2103, "localhost", &fontsize);;
	if (status == GM_SUCCESS)
	{
		gm_layout_set_fontsize(fontsize);
	}
	else
	{
		// fallback on default
		fontsize = gm_layout_get_fontsize();
	}
*/
	started_procs = NULL;
	status =
		gm_network_get_started_procs_from_gappman(2103, "localhost", &started_procs);

	if (status != GM_SUCCESS)
	{
		gm_layout_show_error(status);
	}
	else if (started_procs == NULL)
	{
		gm_layout_show_error_dialog("No programs started by gappman.",
							 NULL, quit_program_callback);
	}

	status = gm_network_get_confpath_from_gappman(2103, "localhost", &gappman_confpath);
	if (status == GM_SUCCESS)
	{
 		///< \todo replace gm_load_conf with gm_network_get_programs_from_gappman
  	if (gm_load_conf(gappman_confpath) != GM_SUCCESS)
		{
			msg = g_strdup_printf("Could not load gappman configuration file:\n%s\n", gappman_confpath);
			gm_layout_show_error_dialog(msg, NULL, quit_program_callback);
			g_free(msg);
		}
	}
	else
	{
		gm_layout_show_error_dialog
			("Could not retrieve gappman configuration file\n",
			 NULL, quit_program_callback);
	}

/* 
	// max column width is 50% of dialog width
	program_width = dialog_width / 2;
	row_height = calculate_row_height(dialog_height);
*/

		
	vbox = create_menu(started_procs);
	if ( vbox != NULL )
	{
		gtk_container_add(GTK_CONTAINER(mainwin), vbox);
		gtk_widget_show(vbox);
		gtk_widget_show(mainwin);
	}

	gtk_main();

	quit_program();

	return 0;
}
