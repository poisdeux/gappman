/***
 * \file appmanager.c
 *
 *
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 */

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <glib/gprintf.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>
#include <getopt.h>
#include <sys/socket.h>
#include <string.h>
#include <gm_changeresolution.h>
#include "listener.h"
#include <gm_parseconf.h>
#include <gm_layout.h>
#include <gm_generic.h>
#include "appmanager.h"
#include <pthread.h>

struct appwidgetinfo* global_appw;
menu_elements *programs;

static int KEEP_BELOW=0;
static int WINDOWED=0;
static int screen_width=-1;
static int screen_height=-1;
static int window_width=-1;
static int window_height=-1;

struct appm_alignment
{
    float x;
    float y;
};

void update_resolution(gchar* programname, int width, int height)
{
    menu_elements* elt = NULL;
    if ( programname != NULL )
    {
        elt = gm_search_elt_by_name(programname, programs);
        if ( elt != NULL )
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

struct appwidgetinfo* get_started_apps()
{
    return global_appw;
}

/**
* \brief Checks if the application is still responding and enables the button when it doesn't respond.
* \param appw appwidgetinfo structure which holds the application's button widget and the PID of the running application.
* \return TRUE if application responds, FALSE if not.
*/
static gint check_app_status(struct appwidgetinfo* local_appw)
{
    int status;
    FILE *fp;
    struct appwidgetinfo* tmp;
    struct menu_element* elt;

    waitpid(local_appw->PID, &(local_appw->status), WNOHANG);

    // Check if process is still running by sending a 0 signal
    // and check if process does not respond
    if ( kill(local_appw->PID, 0) == -1 )
    {
        if (GTK_IS_WIDGET(local_appw->widget))
        {
            //Enable button
            gtk_widget_set_sensitive(GTK_WIDGET(local_appw->widget), TRUE);
        }

        //remove local_appw from list
        if ((local_appw->prev == NULL) && (local_appw->next == NULL))
        {
            //local_appw only element in the list,
            //so we reset the list.
            global_appw = NULL;
            //change resolution back to gappman menu resolution
            gm_changeresolution(screen_width, screen_height);
        }
        else
        {
            if (local_appw->prev != NULL)
            {
                tmp = local_appw->prev;
                tmp->next = local_appw->next;
                //we may need to change resolution to
                //local_appw->prev if local_appw was
                //last program started
                elt = tmp->menu_elt;
            }

            if (local_appw->next != NULL)
            {
                tmp = local_appw->next;
                tmp->prev = local_appw->prev;
                //local_appw was not the last program
                //started so we leave resolution unchanged
                elt = NULL;
            }
            else
            {
                //local_appw is last element
                //we therefore need to relocate
                //the global_appw pointer
                global_appw = local_appw->prev;
            }

            if ( elt != NULL )
            {
                if ( elt->app_width > 0 && elt->app_height > 0 )
                {
                    gm_changeresolution(elt->app_width, elt->app_height);
                }
            }
        }

        //No need for local_appw anymore
        free(local_appw);

        //Stop glib timer
        return FALSE;
    }
    //Continue glib timer
    return TRUE;
}

/**
* \brief Creates an appwidgetinfo structure
* \param PID the process ID of the application
* \param widget pointer to the GtkWidget which belongs to the button of the application
* \param name programname with PID
*/
static void create_new_appwidgetinfo(int PID, GtkWidget *widget, struct menu_element *elt)
{
    struct appwidgetinfo *local_appw;
    local_appw = (struct appwidgetinfo *) malloc(sizeof(struct appwidgetinfo));
    if ( local_appw != NULL)
    {
        local_appw->PID = PID;
        local_appw->menu_elt = elt;
        local_appw->widget = widget;
        local_appw->prev = global_appw;
        local_appw->next = NULL;
        if ( global_appw != NULL )
        {
            global_appw->next = local_appw;
        }
        //shift global global_appw pointer so it always points to
        //last started process
        global_appw = local_appw;
    }
}

/**
* \brief Starts the program with any specified arguments by forking and letting the child (fork) start the program
* \param widget pointer to GtkWidget of the button that was pressed to start the application
* \param elt pointer to the menu_element structure for the application that needs to be started
* \return gboolean FALSE if the fork failed or the program's executable could not be found. TRUE if fork succeeded and program was found.
*/
static gboolean startprogram( GtkWidget *widget, menu_elements *elt )
{
    char **args;
    int i;
    int status;
    int ret;
    __pid_t childpid;
    FILE *fp;

    /**
      Create argument list. First element should be the filename
      of the executable and last element needs to be NULL.
      see man exec for more details
    */
    args = (char **) malloc((elt->numArguments + 2)* sizeof(char *));
    args[0] = (char *) elt->exec;
    for ( i = 0; i < elt->numArguments; i++ )
    {
        args[i+1] = elt->args[i];
    }
    args[i+1] = NULL;

    fp = fopen((char *) elt->exec,"r");
    if ( fp )
    {
        //Disable button
        gtk_widget_set_sensitive(GTK_WIDGET(widget), FALSE);

        fclose(fp);

        if ( elt->app_width > 0 && elt->app_height > 0 )
        {
            gm_changeresolution(elt->app_width, elt->app_height);
        }

        childpid = fork();
        if ( childpid == 0 )
        {
            execvp((char *) elt->exec, args);
            _exit(0);
        }
        else if (  childpid < 0 )
        {
            g_warning("Failed to fork!\n");
            return FALSE;
        }
        else
        {
            create_new_appwidgetinfo(childpid, widget, elt);
            g_timeout_add(1000, (GSourceFunc) check_app_status, (gpointer) global_appw);
        }
    }
    else
    {
        g_warning("File: %s not found!\n", (char *) elt->exec);
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
static gboolean process_startprogram_event ( GtkWidget *widget, GdkEvent *event, menu_elements *elt )
{

    //Only start program  if spacebar or mousebutton is pressed
    if ( ((GdkEventKey*)event)->keyval == 32 || ((GdkEventButton*)event)->button == 1)
    {
        startprogram( widget, elt );
    }

    return FALSE;
}

static void usage()
{
    printf("usage: appmanager [--keep-below] [--width <WIDTHINPIXELS>] [--height <HEIGHTINPIXELS>] [--conffile <FILENAME>] [--gtkrc <GTKRCFILENAME>] [--windowed]\n");
    printf("");
    printf("--keep-below:\t\tKeeps the window at the bottom of the window manager's stack\n");
    printf("--width <WIDTHINPIXELS>:\t\twidth of the main window (default: screen width)\n");
    printf("--height <HEIGHTINPIXELS:\t\theight of the main window (default: screen height)\n");
    printf("--conffile <FILENAME>:\t\t configuration file specifying the program and actions (default: ./conf.xml)\n");
    printf("--gtkrc <GTKRCFILENAME>:\t\t gtk configuration file which can be used for themeing\n");
    printf("--windowed:\t\t creates a border around the window\n");
}

/**
* \brief Check all elements in elts if the integer autostart is set to 1 and start the
*  corresponding program
*/
static void autostartprograms( menu_elements *elts )
{
    menu_elements *next, *cur;

    cur = elts;

    while ( cur != NULL )
    {
        if ( cur->autostart == 1 )
        {
            startprogram( NULL, cur );
        }
        cur = cur->next;
    }
}

/**
* \brief callback function to quit the program
* \param *widget pointer to widget to destroy
* \param data mandatory argument for callback function, may be NULL.
*/
static void destroy( GtkWidget *widget,
                     gpointer   data )
{
    gtk_main_quit ();
}

/**
*	\brief stops the elements in the panel
* \param *panel pointer to the menu_elements structures holding the panel elementts
*/
static void stop_panel (menu_elements *panel)
{
    while (panel != NULL )
    {
		g_debug("Stopping panel element");
        if (panel->gm_module_stop != NULL)
        {
            if (!panel->gm_module_stop() != GM_SUCCES)
            {
                g_error("Failed to stop thread");
            }
        }
        panel = panel->next;
    }
}


/**
*	\brief starts the elements in the panel
* \param *panel pointer to the menu_elements structures holding the panel elementts
*/
static void start_panel (menu_elements *panel)
{
    while (panel != NULL )
    {
        if (panel->gm_module_start != GM_SUCCES)
        {
            if (!g_thread_create((GThreadFunc) panel->gm_module_start, NULL, FALSE, NULL))
            {
                g_error("Failed to create thread");
            }
        }
        panel = panel->next;
    }
}

static void align_buttonbox (GtkWidget *hbox_top, GtkWidget *hbox_middle, GtkWidget *hbox_bottom, GtkWidget *buttonbox, menu_elements *elts)
{
    GtkWidget *hor_align;
    int box_width;
    int box_height;

    box_width = gm_calculate_box_length(window_width, elts->menu_width);
    box_height = gm_calculate_box_length(window_height, elts->menu_height);
    //vertical alignment is calculated by dividing elts->vert_alignment by 2
    //this results in hbox_top having 0.0, hbox_middle 0,5, and hbox_bottom 1.0
    //to have the widgets aligned respectively to the top, center, or bottom
    hor_align = gtk_alignment_new( *elts->hor_alignment, (float) *elts->vert_alignment/2, (float) box_width/window_width, (float) box_height/window_height);
    gtk_container_add (GTK_CONTAINER (hor_align), buttonbox);
    gtk_widget_show(hor_align);

    switch ( *elts->vert_alignment )
    {
    case 0:
        gtk_container_add (GTK_CONTAINER (hbox_top), hor_align);
        break;;
    case 1:
        gtk_container_add (GTK_CONTAINER (hbox_middle), hor_align);
        break;;
    case 2:
        gtk_container_add (GTK_CONTAINER (hbox_bottom), hor_align);
        break;;
    }
}

/**
* \brief main function setting up the UI
*/
int main (int argc, char **argv)
{
    GdkScreen *screen;
    GdkWindow * rootwin;
    GdkPixbuf *pixbuf_bg;
    GtkWidget *window_bg;
    GtkWidget *picture_bg;
    GtkWidget *mainwin;
    GtkWidget *buttonbox;
    GtkWidget *hbox_top;
    GtkWidget *hbox_middle;
    GtkWidget *hbox_bottom;
    GtkWidget *vbox;
    GtkStyle  *style;
    menu_elements *actions;
    menu_elements *panel;
    const char* conffile = "./conf.xml";
    const char* bgimage = NULL;
    int c;
    GIOChannel* gio;

    //Needs to be called before any another glib function
    if (!g_thread_supported ())
    {
        g_message("Threads supported");
        g_thread_init (NULL);
        gdk_threads_init();
    }

    gtk_init (&argc, &argv);
    while (1) {
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

        switch (c) {
        case 'w':
            window_width=atoi(optarg);
            break;
        case 'h':
            window_height=atoi(optarg);
            break;
        case 'c':
            conffile=optarg;
            break;
        case 'r':
            gtk_rc_parse (optarg);
            break;
        case 'b':
            KEEP_BELOW=1;
            break;
        case 'j':
            WINDOWED=1;
            break;
        default:
            usage();
            return 0;
        }
    }


	//set confpath so other programs can retrieve
	//the configuration file gappman used
	gappman_set_confpath(conffile);

    /** INIT */
    global_appw = NULL;

    /** Load configuration elements */
    gm_load_conf(conffile);
    programs = gm_get_programs();
    actions = gm_get_actions();
    panel = gm_get_panel();

    screen = gdk_screen_get_default ();
    screen_width =  gdk_screen_get_width (screen);
    screen_height = gdk_screen_get_height (screen);

		if (window_width == -1)
			window_width = screen_width;
		if (window_height == -1)
			window_height = screen_height;


    gm_set_window_geometry(window_width, window_height);

    mainwin = gtk_window_new (GTK_WINDOW_TOPLEVEL);

		gtk_widget_set_name(mainwin, "gm_mainwindow");

    //Keep the main window below all other windows
    if (KEEP_BELOW)
    {
        gtk_window_set_keep_below(GTK_WINDOW (mainwin), TRUE);
    }

    //Make window transparent
    //  gtk_window_set_opacity (GTK_WINDOW (mainwin), 0.0);

    if (!WINDOWED)
    {
        //Remove border
        gtk_window_set_decorated (GTK_WINDOW (mainwin), FALSE);
    }
    else
    {
        g_signal_connect (G_OBJECT (mainwin), "delete_event",
                          G_CALLBACK (destroy), NULL);
        g_signal_connect (G_OBJECT (mainwin), "destroy",
                          G_CALLBACK (destroy), NULL);
    }

    gtk_window_set_default_size (GTK_WINDOW (mainwin), window_width, window_height);

    vbox = gtk_vbox_new (FALSE, 0);

    hbox_top = gtk_hbox_new (FALSE, 0);
    hbox_middle = gtk_hbox_new (FALSE, 0);
    hbox_bottom = gtk_hbox_new (FALSE, 0);

    if ( actions != NULL )
    {
        buttonbox = gm_create_buttonbox( actions, &process_startprogram_event );
        align_buttonbox(hbox_top, hbox_middle, hbox_bottom, buttonbox, actions);
        gtk_widget_show (buttonbox);
    }

    if ( programs != NULL )
    {
        buttonbox = gm_create_buttonbox( programs, &process_startprogram_event );
        align_buttonbox(hbox_top, hbox_middle, hbox_bottom, buttonbox, programs);
        gtk_widget_show (buttonbox);
    }

    if ( panel != NULL )
    {
        buttonbox = gm_create_panel( panel );
        if( buttonbox != NULL )
				{
					align_buttonbox(hbox_top, hbox_middle, hbox_bottom, buttonbox, panel);
        	gtk_widget_show (buttonbox);
        	start_panel( panel );
				}
				else
				{
					panel = NULL;
				}
    }
    gtk_container_add (GTK_CONTAINER (vbox), hbox_top);
    gtk_container_add (GTK_CONTAINER (vbox), hbox_middle);
    gtk_container_add (GTK_CONTAINER (vbox), hbox_bottom);
    gtk_container_add (GTK_CONTAINER (mainwin), vbox);
    gtk_widget_show(hbox_top);
    gtk_widget_show(hbox_middle);
    gtk_widget_show(hbox_bottom);
    gtk_widget_show (vbox);
    gtk_widget_show (mainwin);

    autostartprograms( programs );

    gappman_start_listener(mainwin);

	stop_panel( panel );
    gdk_threads_enter();
    gtk_main ();
    gdk_threads_leave();


    gappman_close_listener(NULL);
    g_message("Closing up. Goodbye\n");
    gm_free_menu_elements( programs );
    gm_free_menu_elements( actions );
    gm_free_menu_elements( panel );

    return 0;
}

