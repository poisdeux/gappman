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
static char *color[4] = { "green", "orange", "red", "yellow" };
static char *statusarray[6] =
	{ "running", "sleeping", "stopped", "waiting", "zombie", "paging" };
static int fontsize;
static gm_menu *programs;
static gm_menu *actions;

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
  gtk_main_quit();
}

static void quit_program_callback(GtkWidget * dummy, GdkEvent * event, void *data)
{
	if(gm_layout_check_key(event))
	{
		quit_program();
	}
}

static int get_status(int PID)
{
	char *proc_string = malloc((strlen("/proc/stat") + 6) * sizeof(char));
	gchar *contents = NULL;
	gchar **contentssplit = NULL;
	gsize length;
	GError *gerror = NULL;
	gchar *status = NULL;
	enum pid_status ret_status;

	ret_status = NONE;

	g_sprintf(proc_string, "/proc/%d/stat", PID);
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

	if (((GdkEventKey *) event)->keyval != 32
		&& ((GdkEventButton *) event)->button != 1)
		return FALSE;

	status = get_status(elt->pid);
	switch (status)
	{
	case RUNNING:
		kill(elt->pid, 15);
		break;;
	case SLEEPING:
		kill(elt->pid, 15);
		break;;
	case TRACED_OR_STOPPED:
		kill(elt->pid, 9);
		break;;
	case UNINTERRUPTIBLE_DISK_SLEEP:
		kill(elt->pid, 9);
		break;;
	case ZOMBIE:
		// What to do with zombies? Kill parent?
		g_debug("Oh my god! It's a zombie. Who's your daddy!?\n");
		return FALSE;;
	case NONE:
		return TRUE;;
	default:
		g_warning("Huh? What should I do with status %d for proces %d\n",
				  status, elt->pid);
		return FALSE;;
	}

	// disable kill button
	gtk_widget_set_sensitive(widget, FALSE);

	wait_count = 0;
	while ((get_status(elt->pid) != NONE) && (wait_count < 10))
	{
		sleep(1);
		wait_count++;
	}

	if ( status == NONE )
	{
		// remove program from main window
		gtk_widget_set_sensitive(elt->widget, FALSE);
		gtk_widget_destroy(g_object_get_data((GObject *) widget, "window"));
	}
	if (status != get_status(elt->pid))
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
	gchar *markup;
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
	if (((GdkEventKey *) event)->keyval == 32
		|| ((GdkEventButton *) event)->button == 1)
	{
		gtk_widget_set_sensitive(GTK_WIDGET(widget), FALSE);
		showprocessdialog(elt);
		gtk_widget_set_sensitive(GTK_WIDGET(widget), TRUE);
	}
}


static void add_row(gm_menu_element * elt, gint width, gint height, GtkWidget *vbox)
{
	GtkWidget *hbox;
	GtkWidget *statuslabel;
	GtkWidget *separator;
	gchar *markup;
	GtkWidget *alignment;
	int status;

	hbox = gtk_hbox_new(FALSE, 10);

	elt->widget =
		gm_layout_create_button(elt, width, height, process_startprogram_event);

	statuslabel = gtk_label_new("");
	status = get_status(elt->pid);

	markup =
		g_markup_printf_escaped
		("<span size=\"%d\" foreground=\"%s\">%s</span>", fontsize,
		 color[status], statusarray[status]);
	gtk_label_set_markup(GTK_LABEL(statuslabel), markup);
	g_free(markup);

	// right justify the labeltext
	alignment =
		gtk_alignment_new((gfloat) 1.0, (gfloat) 0.5, (gfloat) 0.0,
						  (gfloat) 0.0);
	gtk_container_add(GTK_CONTAINER(alignment), statuslabel);
	gtk_widget_show(statuslabel);

	gtk_box_pack_start(GTK_BOX(hbox), elt->widget, TRUE, TRUE, 0);
	gtk_widget_show(elt->widget);
	gtk_container_add(GTK_CONTAINER(hbox), alignment);
	gtk_widget_show(alignment);

	gtk_container_add(GTK_CONTAINER(vbox), hbox);
	gtk_widget_show(hbox);

	separator = gtk_hseparator_new();
	gtk_container_add(GTK_CONTAINER(vbox), separator);
	gtk_widget_show(separator);
}

gint calculate_row_height( gint dialog_height)
{
	gint accumulated_amount = 0;
	gm_menu *programs;
	gm_menu *actions;

	programs = gm_get_programs();
  if (programs != NULL)
	{
		accumulated_amount += programs->amount_of_elements;
	}

	actions = gm_get_actions();
	if (actions != NULL)
	{
		accumulated_amount += actions->amount_of_elements;
	}

	return (dialog_height / accumulated_amount);
}

GtkWidget *create_menu(gint width, gint height, struct proceslist *started_procs)
{
	gm_menu *programs;
	gm_menu *actions;
	gm_menu_element *elt;
	struct proceslist *started_procs_tmp;
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *button;
	gint no_progsacts_found;
	gint mypid;
	

	vbox = gtk_vbox_new(FALSE, 10);

	programs = gm_get_programs();
	actions = gm_get_actions();

	started_procs_tmp = started_procs;
	mypid = getpid();
	no_progsacts_found = 1;

	while (started_procs != NULL)
	{
		if( started_procs->pid == mypid )
		{
			started_procs = started_procs->prev;
			continue;
		}

		elt = gm_menu_search_elt_by_name(started_procs->name, programs);
		if( elt != NULL )
		{
			no_progsacts_found = 0;
			gm_menu_element_set_pid(started_procs->pid, elt);
			add_row(elt, width, height, vbox);
		}

		elt = gm_menu_search_elt_by_name(started_procs->name, actions);
		if( elt != NULL )
		{
			no_progsacts_found = 0;
			gm_menu_element_set_pid(started_procs->pid, elt);
			add_row(elt, width, height, vbox);
		}

		started_procs = started_procs->prev;
	}

	gm_network_free_proceslist(started_procs);

	if ( no_progsacts_found )
	{
		gm_layout_show_error_dialog("No programs or actions started.", NULL, quit_program_callback);
		return NULL;
	}
	else
	{
		hbox = gtk_hbox_new(FALSE, 10);
		// cancel button
		button =
			gm_layout_create_label_button("Cancel", (void *)quit_program_callback,
								   NULL);
		gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
		gtk_widget_show(button);
	
		gtk_container_add(GTK_CONTAINER(vbox), hbox);
		gtk_widget_show(hbox);
		return vbox;
	}
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
 
	// max column width is 50% of dialog width
	program_width = dialog_width / 2;
	row_height = calculate_row_height(dialog_height);
	
	vbox = create_menu(program_width, row_height, started_procs);
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
