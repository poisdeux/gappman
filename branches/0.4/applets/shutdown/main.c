/***
 * \file main.c
 *
 *
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
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
#include <gm_generic.h>

static int WINDOWED = 0;

static void usage()
{
    printf("usage: shutdown [--help] [--width <WIDTHINPIXELS>] [--height <HEIGHTINPIXELS>] [--conffile <FILENAME>] [--gtkrc <GTKRCFILENAME>] [--windowed]\n");
    printf("");
    printf("--help:\t\tshows this help text\n");
    printf("--width <WIDTHINPIXELS>:\t\twidth of the main window (default: screen width / 9)\n");
    printf("--height <HEIGHTINPIXELS:\t\theight of the main window (default: screen height / 9)\n");
    printf("--conffile <FILENAME>:\t\t configuration file specifying the program and actions (default: /etc/gappman/shutdown.xml)\n");
    printf("--gtkrc <GTKRCFILENAME>:\t\t gtk configuration file which can be used for themeing\n");
    printf("--windowed:\t\t creates a border around the window\n");

}

static gboolean startprogram( GtkWidget *widget, menu_elements *elt )
{
    char **args;
    int i;
    int status;
    int ret;
    struct appwidgetinfo* appw;
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

        childpid = fork();
        if ( childpid == 0 )
        {
            execvp((char *) elt->exec, args);
            _exit(0);
        }
        else if (  childpid < 0 )
        {
            printf("Failed to fork!\n");
            return FALSE;
        }
    }
    else
    {
        printf("File: %s not found!\n", (char *) elt->exec);
    }

    free(args);
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
* \brief main function setting up the UI
*/
int main (int argc, char **argv)
{
    GdkScreen *screen;
    GtkWidget *button;
    GtkWidget *vbox;
    GtkWidget *hbox;
    GtkWidget *align;
    GtkWidget *mainwin;
    menu_elements *actions;
    const char* conffile = "/etc/gappman/shutdown.xml";
    const char* rcfile = "";
    int c;
    int fontsize;

    gtk_init (&argc, &argv);

    while (1) {
        int option_index = 0;
        static struct option long_options[] = {
            {"conffile", 1, 0, 'c'},
            {"help", 0, 0, 'i'},
            {"gtkrc", 1, 0, 'r'},
            {"windowed", 0, 0, 'j'},
            {0, 0, 0, 0}
        };
        c = getopt_long(argc, argv, "w:h:c:d:r:ij",
                        long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
        case 'c':
            conffile=optarg;
            break;
        case 'r':
            gtk_rc_parse (optarg);
            break;
        case 'j':
            WINDOWED=1;
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

    screen = gdk_screen_get_default ();

    mainwin = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    gtk_window_set_position(GTK_WINDOW (mainwin), GTK_WIN_POS_CENTER);

    //Make window transparent
    //gtk_window_set_opacity (GTK_WINDOW (mainwin), 0.8);

    //Remove border
    if ( !WINDOWED )
    {
        gtk_window_set_decorated (GTK_WINDOW (mainwin), FALSE);
    }
    else
    {
        gtk_window_set_decorated (GTK_WINDOW (mainwin), TRUE);
        g_signal_connect (G_OBJECT (mainwin), "delete_event",
                          G_CALLBACK (destroy), NULL);
        g_signal_connect (G_OBJECT (mainwin), "destroy",
                          G_CALLBACK (destroy), NULL);
    }
    vbox = gtk_vbox_new (FALSE, 10);

    //We do not get the fontsize from gappman but let the buttonbox
    //determine the right fontsize
    if ( actions != NULL )
    {
        align = gm_create_buttonbox( actions, &process_startprogram_event );
        gtk_container_add (GTK_CONTAINER (vbox), align);
        gtk_widget_show (align);
    }

    hbox = gtk_hbox_new (FALSE, 10);

    // cancel button
    button = gm_create_label_button("Cancel", gtk_main_quit, NULL);
    gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);
    gtk_widget_show(button);

    gtk_container_add (GTK_CONTAINER (vbox), hbox);
    gtk_widget_show (hbox);

    gtk_container_add (GTK_CONTAINER (mainwin), vbox);
    gtk_widget_show (vbox);

    //make sure the widget grabs keyboard and mouse focus
    gtk_grab_add(mainwin);

    gtk_widget_show (mainwin);

    //gtk_window_set_focus(GTK_WINDOW (mainwin), button);
    gtk_main ();

    gm_free_menu_elements( actions );

    return 0;
}

