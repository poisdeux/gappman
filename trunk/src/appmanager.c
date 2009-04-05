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

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>
#include <getopt.h>
#include <sys/socket.h>
#include <string.h>
#include "changeresolution.h"
#include "listener.h"
#include "../libs/parseconf/parseconf.h"
#include "../libs/layout/layout.h"


static int DEBUG=0;
static int KEEP_BELOW=0;
static int WINDOWED=0;
static int screen_width=-1;
static int screen_height=-1;

struct appm_alignment
{
  float x;
  float y;
};

struct appwidgetinfo
{
  int PID; //<! Process ID of running app (child replaced through execvp)
  GtkWidget *widget;
};

/**
* \brief Checks if the application is still responding and enables the button when it doesn't respond.
* \param appw appwidgetinfo structure which holds the application's button widget and the PID of the running application.
* \return TRUE if application responds, FALSE if not.
*/ 
static gint check_app_status(struct appwidgetinfo* appw)
{
  int status;
  FILE *fp;
  
  waitpid(appw->PID, &status, WNOHANG);
  if(DEBUG == 2)
  {
    printf("Status of PID: %d is %d\n", appw->PID, status);
  }
  
  // Check if process is still running by sending a 0 signal
  // and check if process does not respond
  if( kill(appw->PID, 0) == -1 )
  {
    if(GTK_IS_WIDGET(appw->widget))
    {
      //Enable button
      gtk_widget_set_sensitive(GTK_WIDGET(appw->widget), TRUE);
    }
    
    //No need for appw anymore
    free(appw);
    
    //Change resolution back to menu resolution
    changeresolution(screen_width, screen_height);
    
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
* \return pointer to appwidgetinfo structure
*/
static struct appwidgetinfo* create_new_appwidgetinfo(int PID, GtkWidget *widget)
{
  struct appwidgetinfo *appw;
  appw = (struct appwidgetinfo *) malloc(sizeof(struct appwidgetinfo));
  appw->PID = PID;
  appw->widget = widget;
  return appw;
}

/**
* \brief function to create and initialize an appm_alignment struct
* \param *alignment Pointer to an appm_alignment struct
*/
static struct appm_alignment * create_appm_alignment_struct()
{
  struct appm_alignment *alignment;
  alignment = (struct appm_alignment *) malloc(sizeof(struct appm_alignment));
  alignment->x = -1.0; //<! set to negative value to be able to test if value has been set
  alignment->y = -1.0; //<! set to negative value to be able to test if value has been set
  return alignment;
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
  if( fp )
  {
    //Disable button
    gtk_widget_set_sensitive(GTK_WIDGET(widget), FALSE);

    fclose(fp);
    
    if( elt->app_width > 0 && elt->app_height > 0 )
    {
      changeresolution(elt->app_width, elt->app_height);
    }
      
    childpid = fork();
    if ( childpid == 0 )
    {
      if(DEBUG == 2)
      {
        printf("Executing: %s with args: ", elt->exec);
        for ( i = 0; i < elt->numArguments; i++ )
        {
          printf("%s ", args[i+1]);
        }
        printf("\n");
      }
      
      execvp((char *) elt->exec, args);
      _exit(0);
    }
    else if (  childpid < 0 )
    {
      printf("Failed to fork!\n");
      return FALSE;
    }
    else
    {
      appw = create_new_appwidgetinfo(childpid, widget);
      g_timeout_add(1000, (GSourceFunc) check_app_status, (gpointer) appw);
    }
  }
  else 
  {
    printf("File: %s not found!\n", (char *) elt->exec);
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
  
  if (DEBUG == 2)
  {
    printf("process_startprogram_event\n");
  }

  if(DEBUG > 1)
  {
    if ( event->type == GDK_KEY_RELEASE )
    {
      printf("%s key pressed and has value %d\n", ((GdkEventKey*)event)->string, ((GdkEventKey*)event)->keyval);
    }
  }
  
  //Only start program  if spacebar or mousebutton is pressed
  if( ((GdkEventKey*)event)->keyval == 32 || ((GdkEventButton*)event)->button == 1)
  {
    startprogram( widget, elt );
  }

  return FALSE;
}

static void usage()
{
  printf("usage: appmanager [--keep-below] [--debug <LEVEL>] [--width <WIDTHINPIXELS>] [--height <HEIGHTINPIXELS>] [--conffile <FILENAME>] [--gtkrc <GTKRCFILENAME>] [--windowed]\n");
  printf("");
  printf("--keep-below:\t\tKeeps the window at the bottom of the window manager's stack\n");
  printf("--debug <LEVEL>:\t\tsets verbosity leven\n");
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
  
  while( cur != NULL )
  {
    if ( cur->autostart == 1 )
    {
      if ( DEBUG > 0 )
      {
        printf("Autostarting %s\n", cur->name);
      }
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
  GtkWidget *align;
  GtkWidget *vbox;
  GtkStyle  *style;
  menu_elements *actions;
  menu_elements *programs;
  const char* conffile = "./conf.xml";
  const char* bgimage = NULL;
  int c;

  gtk_init (&argc, &argv);

  while (1) {
      int option_index = 0;
      static struct option long_options[] = {
          {"width", 1, 0, 'w'},
          {"height", 1, 0, 'h'},
          {"conffile", 1, 0, 'c'},
          {"debug", 1, 0, 'd'},
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
          screen_width=atoi(optarg);
          break;
      case 'h':
          screen_height=atoi(optarg);
          break;
      case 'c':
          conffile=optarg;
          break;
      case 'd':
          DEBUG=atoi(optarg);
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

  /** Load configuration elements */
  loadConf(conffile);
  programs = getPrograms();
  actions = getActions();

   screen = gdk_screen_get_default ();
   if ( screen_width == -1 )
   {
        screen_width =  gdk_screen_get_width (screen);
   }
   if ( screen_height == -1 )
   {
        screen_height =  gdk_screen_get_height (screen);
   }

  mainwin = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  
  //Keep the main window below all other windows
  if(KEEP_BELOW)
  {
    gtk_window_set_keep_below(GTK_WINDOW (mainwin), TRUE);
  }
  
  //Make window transparent
  //  gtk_window_set_opacity (GTK_WINDOW (mainwin), 0.0);
  
  if(!WINDOWED)
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
  
  gtk_window_set_default_size (GTK_WINDOW (mainwin), screen_width, screen_height);

  vbox = gtk_vbox_new (TRUE, 0);

  if ( actions != NULL )
  {
    align = createbuttons( actions, screen_width, screen_height , &process_startprogram_event );
    gtk_container_add (GTK_CONTAINER (vbox), align);
    gtk_widget_show (align);
  }

  if ( programs != NULL )
  {
    align = createbuttons( programs, screen_width, screen_height, &process_startprogram_event );
    gtk_container_add (GTK_CONTAINER (vbox), align);
    gtk_widget_show (align);
  }

  gtk_container_add (GTK_CONTAINER (mainwin), vbox);
  gtk_widget_show (vbox);
  gtk_widget_show (mainwin);

  autostartprograms( programs );

	if ( ! gappman_start_listener("localhost", 2103) )
	{
		fprintf(stderr, "Error: could not start listener.\n");
	}
  
  gtk_main ();

  freeMenuElements( programs );
  freeMenuElements( actions );
  
	return 0;
}

