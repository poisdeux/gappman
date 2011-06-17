/**
 * \file appmanager.c
 * \brief The main application manager better known as gappman
 *
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 *
 * \todo Be able to popup main menu when other apps have focus.
 *       This should probably be solved in the window manager used and not 
 *    	 solved by gappman.
 * \todo Unit-testing with http://klee.llvm.org/
 * \todo Implement support for keybindings. For instance to start specific applets.
 * \todo Startup indicator when starting programs. We could do this through an animated mouse-pointer or animate the program-button.
 * \todo Make it possible to let gappman redraw the main window when the screen resolution changes
 * \todo align_buttonbox calls gm_calculate_boxlength. This is also called in gm_layout when creating the box. We might want to capture the values in struct menu to prevent recalculation.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H
#include "appmanager.h"
#include <glib.h>
#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <glib/gprintf.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>
#include <getopt.h>
#include <errno.h>
#include <sys/socket.h>
#include <string.h>
#include <gm_changeresolution.h>
#include <gm_parseconf.h>
#include <gm_layout.h>
#include <gm_generic.h>
#include "listener.h"

struct process_info *started_apps;	// /< holds the currently started apps
struct menu *programs;		// /< list of all programs gappman manages.
								// Currently only programs need to be global
								// as only programs have meta-info that can be 
								// updated. E.g. resolution updates for a
								// specific program. 

static int KEEP_BELOW = 0;
static int WINDOWED = 0;
static int screen_width = -1;
static int screen_height = -1;
static int window_width = -1;
static int window_height = -1;

void appmanager_update_resolution(gchar * programname, int width, int height)
{
	struct menu_element *elt = NULL;
	if (programname != NULL)
	{
		elt = gm_search_elt_by_name(programname, programs);
		if (elt != NULL)
		{
			elt->app_width = width;
			elt->app_height = height;
		}
	}
	else
	{
		screen_width = width;
		screen_height = height;
	}
}

struct process_info *appmanager_get_started_apps()
{
	return started_apps;
}

/**
* \brief Checks if the application is still responding and enables the button when it doesn't respond.
* \param local_appw process_info structure which holds the application's button widget and the PID of the running application.
* \return TRUE if application responds, FALSE if not.
*/
static gint check_app_status(struct process_info *local_appw)
{
	struct process_info *tmp;
	struct menu_element *elt;

	waitpid(local_appw->PID, &(local_appw->status), WNOHANG);

	// Check if process is still running by sending a 0 signal
	// and check if process does not respond
	if (kill(local_appw->PID, 0) == -1)
	{
		if (GTK_IS_WIDGET(local_appw->menu_elt->widget))
		{
			// Enable button
			gtk_widget_set_sensitive(GTK_WIDGET(local_appw->menu_elt->widget), TRUE);
		}

		// remove local_appw from list
		if ((local_appw->prev == NULL) && (local_appw->next == NULL))
		{
			// local_appw only element in the list,
			// so we reset the list.
			started_apps = NULL;
			// change resolution back to gappman menu resolution
			gm_res_changeresolution(screen_width, screen_height);
		}
		else
		{
			if (local_appw->prev != NULL)
			{
				tmp = local_appw->prev;
				tmp->next = local_appw->next;
				// we may need to change resolution to
				// local_appw->prev if local_appw was
				// last program started
				elt = tmp->menu_elt;
			}

			if (local_appw->next != NULL)
			{
				tmp = local_appw->next;
				tmp->prev = local_appw->prev;
				// local_appw was not the last program
				// started so we leave resolution unchanged
				elt = NULL;
			}
			else
			{
				// local_appw is last element
				// we therefore need to relocate
				// the started_apps pointer
				started_apps = local_appw->prev;
			}

			if (elt != NULL)
			{
				if (elt->app_width > 0 && elt->app_height > 0)
				{
					gm_res_changeresolution(elt->app_width, elt->app_height);
				}
			}
		}

		// No need for local_appw anymore
		free(local_appw);

		// Stop glib timer
		return FALSE;
	}
	// Continue glib timer
	return TRUE;
}

/**
* \brief Creates an process_info structure
* \param PID the process ID of the application
* \param widget pointer to the GtkWidget which belongs to the button of the application
* \param *elt menu_element of the application the new process_info must be created for 
*/
static void create_new_process_info_struct(int PID, struct menu_element *elt)
{
	struct process_info *tmp;
	tmp = (struct process_info *)malloc(sizeof(struct process_info));
	if (tmp != NULL)
	{
		tmp->PID = PID;
		tmp->menu_elt = elt;
		tmp->prev = started_apps;
		tmp->next = NULL;
		if (started_apps != NULL)
		{
			started_apps->next = tmp;
		}
		// shift global started_apps pointer so it always points to
		// last started process
		started_apps = tmp;
	}
}

/**
* \brief Starts the program with any specified arguments by forking and letting the child (fork) start the program
* \param widget pointer to GtkWidget of the button that was pressed to start the application
* \param elt pointer to the menu_element structure for the application that needs to be started
* \return gboolean FALSE if the fork failed or the program's executable could not be found. TRUE if fork succeeded and program was found.
*/
static gboolean startprogram(struct menu_element *elt)
{
	char **args;
	int i;
	__pid_t childpid;
	FILE *fp;

	/**
      Create argument list. First element should be the filename
      of the executable and last element needs to be NULL.
      see man exec for more details
    */
	args = (char **)malloc((elt->numArguments + 2) * sizeof(char *));
	args[0] = (char *)elt->exec;
	for (i = 0; i < elt->numArguments; i++)
	{
		args[i + 1] = elt->args[i];
	}
	args[i + 1] = NULL;

	fp = fopen((char *)elt->exec, "r");
	if (fp)
	{
		// Disable button
		gtk_widget_set_sensitive(elt->widget, FALSE);

		fclose(fp);

		if (elt->app_width > 0 && elt->app_height > 0)
		{
			gm_res_changeresolution(elt->app_width, elt->app_height);
		}

		childpid = fork();
		if (childpid == 0)
		{
			if (execv((char *)elt->exec, args) == -1)
			{
				g_warning("Could not execute %s: errno: %d\n", elt->exec,
						  errno);
				_exit(1);
			}
			_exit(0);
		}
		else if (childpid < 0)
		{
			g_warning("Failed to fork!\n");
			return FALSE;
		}
		else
		{
			create_new_process_info_struct(childpid, elt);
			g_timeout_add(1000, (GSourceFunc) check_app_status,
						  (gpointer) started_apps);
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
									   struct menu_element *elt)
{

	// Only start program if spacebar or mousebutton is pressed
	if (((GdkEventKey *) event)->keyval == 32
		|| ((GdkEventButton *) event)->button == 1)
	{
		startprogram(elt);
	}
}

static void usage()
{
	printf
		("usage: appmanager [--keep-below] [--width <WIDTHINPIXELS>] [--height <HEIGHTINPIXELS>] [--conffile <FILENAME>] [--gtkrc <GTKRCFILENAME>] [--windowed]\n");
	printf("\n");
	printf
		("--keep-below:\t\t\tKeeps the window at the bottom of the window manager's stack\n");
	printf
		("--width <WIDTHINPIXELS>:\twidth of the main window (default: screen width)\n");
	printf
		("--height <HEIGHTINPIXELS:\theight of the main window (default: screen height)\n");
	printf
		("--conffile <FILENAME>:\t\tconfiguration file specifying the program and actions (default: ./conf.xml)\n");
	printf
		("--gtkrc <GTKRCFILENAME>:\tgtk configuration file which can be used for themeing\n");
	printf("--windowed:\t\t\truns gappman in a window\n");
}

/**
* \brief Check all elements in elts if the integer autostart is set to 1 and start the
*  corresponding program
*/
static void autostartprograms(struct menu *dish)
{
	int i;
	for(i = 0; i < dish->amount_of_elements; i++)
	{
		if (dish->elts[i].autostart == 1)
		{
			startprogram(&(dish->elts[i]));
		}
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

/**
*	\brief stops the elements in the panel
* \param *panel pointer to the menu_elements structures holding the panel elementts
*/
static void stop_panel(struct menu *panel)
{
	int i;
	for(i = 0; i < panel->amount_of_elements; i++)
	{
		if (panel->elts[i].gm_module_stop != NULL)
		{
			panel->elts[i].gm_module_stop();
		}
	}
}


/**
*	\brief starts the elements in the panel
* \param *panel pointer to the menu_elements structures holding the panel elementts
*/
static void start_panel(struct menu *panel)
{
	GThread *thread;
	int i;
	
	for(i = 0; i < panel->amount_of_elements; i++)
	{
		if (panel->elts[i].gm_module_start != NULL)
		{
			thread =
				g_thread_create((GThreadFunc) panel->elts[i].gm_module_start, NULL,
								TRUE, NULL);
			if (!thread)
			{
				g_warning("Failed to create thread");
			}
		}
	}
}

static void align_buttonbox(GtkWidget * hbox_top, GtkWidget * hbox_middle,
							GtkWidget * hbox_bottom, GtkWidget * buttonbox,
							struct menu *dish)
{
	GtkWidget *hor_align;
	int box_width;
	int box_height;

	box_width = gm_calculate_box_length(window_width, &(dish->menu_width));
	box_height = gm_calculate_box_length(window_height, &(dish->menu_height));
	hor_align =
		gtk_alignment_new(dish->hor_alignment,
						  (float)dish->vert_alignment / 2,
							0, 0);
	gtk_container_add(GTK_CONTAINER(hor_align), buttonbox);
	gtk_widget_show(hor_align);

	switch (dish->vert_alignment)
	{
	case 0:
		gtk_container_add(GTK_CONTAINER(hbox_top), hor_align);
		break;;
	case 1:
		gtk_container_add(GTK_CONTAINER(hbox_middle), hor_align);
		break;;
	case 2:
		gtk_container_add(GTK_CONTAINER(hbox_bottom), hor_align);
		break;;
	}
}

/**
* \brief main function setting up the UI
*/
int main(int argc, char **argv)
{
	GdkScreen *screen;
	GtkWidget *mainwin;
	GtkWidget *buttonbox;
	GtkWidget *hbox_top;
	GtkWidget *hbox_middle;
	GtkWidget *hbox_bottom;
	GtkWidget *vbox;
	struct menu *actions;
	struct menu *panel;
	const char *conffile = SYSCONFDIR "/conf.xml";
	int c;

	// Needs to be called before any another glib function
	if (!g_thread_supported())
	{
		g_message("Threads supported");
		g_thread_init(NULL);
		gdk_threads_init();
	}

	gtk_init(&argc, &argv);
	while (1)
	{
		int option_index = 0;
		static struct option long_options[] = {
			{"width", 1, 0, 'w'},
			{"height", 1, 0, 'h'},
			{"conffile", 1, 0, 'c'},
			{"help", 0, 0, 'i'},
			{"gtkrc", 1, 0, 'r'},
			{"keep-below", 0, 0, 'b'},
			{"windowed", 0, 0, 'j'},
			{0, 0, 0, 0}
		};

		c = getopt_long(argc, argv, "w:h:c:d:r:ibj",
						long_options, &option_index);
		if (c == -1)
			break;

		switch (c)
		{
		case 'w':
			window_width = atoi(optarg);
			break;
		case 'h':
			window_height = atoi(optarg);
			break;
		case 'c':
			conffile = optarg;
			break;
		case 'r':
			gtk_rc_parse(optarg);
			break;
		case 'b':
			KEEP_BELOW = 1;
			break;
		case 'j':
			WINDOWED = 1;
			break;
		default:
			usage();
			return 0;
		}
	}


	gm_res_init();

	/** INIT */
	started_apps = NULL;

	/** Load configuration elements */
	gm_load_conf(conffile);
	programs = gm_get_programs();
	actions = gm_get_actions();
	panel = gm_get_panel();

	screen = gdk_screen_get_default();
	screen_width = gdk_screen_get_width(screen);
	screen_height = gdk_screen_get_height(screen);

	if (window_width == -1)
		window_width = screen_width;
	if (window_height == -1)
		window_height = screen_height;


	gm_set_window_geometry(window_width, window_height);

	mainwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	gtk_widget_set_name(mainwin, "gm_mainwindow");

	// Keep the main window below all other windows
	if (KEEP_BELOW)
	{
		gtk_window_set_keep_below(GTK_WINDOW(mainwin), TRUE);
	}

	// Make window transparent
	// gtk_window_set_opacity (GTK_WINDOW (mainwin), 0.0);

	if (!WINDOWED)
	{
		// Remove border
		gtk_window_set_decorated(GTK_WINDOW(mainwin), FALSE);
	}
	else
	{
		g_signal_connect(G_OBJECT(mainwin), "delete_event",
						 G_CALLBACK(destroy), NULL);
		g_signal_connect(G_OBJECT(mainwin), "destroy",
						 G_CALLBACK(destroy), NULL);
	}

	gtk_window_set_default_size(GTK_WINDOW(mainwin), window_width,
								window_height);

	vbox = gtk_vbox_new(FALSE, 0);

	hbox_top = gtk_hbox_new(FALSE, 0);
	hbox_middle = gtk_hbox_new(FALSE, 0);
	hbox_bottom = gtk_hbox_new(FALSE, 0);

	if (actions != NULL)
	{
		buttonbox =
			gm_create_menu(actions, &process_startprogram_event);
		align_buttonbox(hbox_top, hbox_middle, hbox_bottom, buttonbox,
						actions);
		gtk_widget_show(buttonbox);
	}

	if (programs != NULL)
	{
		buttonbox =
			gm_create_menu(programs, &process_startprogram_event);
			align_buttonbox(hbox_top, hbox_middle, hbox_bottom, buttonbox,
						programs);
		gtk_widget_show(buttonbox);
	}

	if (panel != NULL)
	{
		buttonbox = gm_create_panel(panel);
		if (buttonbox != NULL)
		{
			align_buttonbox(hbox_top, hbox_middle, hbox_bottom, buttonbox,
							panel);
			gtk_widget_show_all(buttonbox);
			start_panel(panel);
		}
		else
		{
			panel = NULL;
		}
	}
	gtk_container_add(GTK_CONTAINER(vbox), hbox_top);
	gtk_widget_show(hbox_top);
	gtk_container_add(GTK_CONTAINER(vbox), hbox_middle);
	gtk_widget_show(hbox_middle);
	gtk_container_add(GTK_CONTAINER(vbox), hbox_bottom);
	gtk_widget_show(hbox_bottom);
	gtk_container_add(GTK_CONTAINER(mainwin), vbox);
	gtk_widget_show(vbox);

#if !defined(NO_LISTENER)
	// set confpath so other programs can retrieve
	// the configuration file gappman used
	gappman_set_confpath(conffile);
	gappman_start_listener(mainwin);
#else
	g_warning("Gappman compiled without network support");
#endif

	autostartprograms(programs);

	gtk_widget_show(mainwin);

	gdk_threads_enter();
	gtk_main();
	gdk_threads_leave();

	g_message("Closing up.");
	stop_panel(panel);
#if !defined(NO_LISTENER)
	gappman_close_listener();
#endif
	gm_free_menu(programs);
	gm_free_menu(actions);
	gm_free_menu(panel);

	g_message("Goodbye.");
	return 0;
}
