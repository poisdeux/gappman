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
#include <string.h>
#include "changeresolution.h"
#include "../libs/parseconf/parseconf.h"

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
* \brief function to calculate the absolute width based upon the total available width
* \param total_width Total available width for the box element
* \param *box_width Pointer to a struct length holding the length value and type of the box
* \return box width in amount of pixels
*/
static int calculateBoxLength(int total_length, struct length *box_length)
{
  int length;

  if ( box_length->type == PERCENTAGE )
  {
    length = round( (double) total_length * (box_length->value / (double) 100));
  }
  else
  {
    length = box_length->value;
  }

  if ( length > total_length)
  {
    g_warning( "Box length exceeds total length. This might indicate a configuration error." );
  }

  return length;
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

/**
* \brief function that highlights a button 
* \param *widget pointer to the button widget
* \param *event the GdkEvent that occured.
* \param *elt will not be used, but must be defined.
*/
static gboolean press_button ( GtkWidget *widget, GdkEvent *event, menu_elements *elt )
{
  if (DEBUG == 2)
  {
    printf("press_button: event->type: %d\n", event->type);
  }
  gtk_widget_activate(widget);
  return FALSE;
}

/**
* \brief function that highlights a button 
* \param *widget pointer to the button widget
* \param *event the GdkEvent that occured.
* \param *elt will not be used, but must be defined.
*/
static gboolean highlight_button ( GtkWidget *widget, GdkEvent *event, menu_elements *elt )
{
  if (DEBUG == 2)
  {
    printf("highlight_button: event->type: %d\n", event->type);
  }

  gtk_button_set_relief(GTK_BUTTON(widget), GTK_RELIEF_NORMAL);
  gtk_widget_grab_focus(widget);
  return TRUE;
}

/**
* \brief function that de-highlights a button 
* \param *widget pointer to the button widget
* \param *event the GdkEvent that occured.
* \param *elt will not be used, but must be defined.
*/
static gboolean dehighlight_button ( GtkWidget *widget, GdkEvent *event, menu_elements *elt )
{
  if (DEBUG == 2)
  {
    printf("dehighlight_button: event->type: %d\n", event->type);
  }

  gtk_button_set_relief(GTK_BUTTON(widget), GTK_RELIEF_NONE);
  return TRUE;
}

/**
* \brief Calculates the amount of elements per row to evenly spread all elements on a surface of screen_height x screen_width.
* \param screen_height total screen height
* \param screen_width total screen width
* \param amount_of_elements number of elements that should be placed in the screen_height x screen_width area
*/
static int calculateAmountOfElementsPerColumn(int screen_height, int screen_width, int amount_of_elements)
{
  double rows;
  double ratio;
  ratio = (double) MAX(screen_height, screen_width) / (double) MIN(screen_height, screen_width);
  rows = sqrt(amount_of_elements/ratio);
  
  if (DEBUG > 0)
  {
    printf("calculateAmountOfElementsPerColumn: screen_width=%d, screen_height=%d, ratio=%f, rows=%f, round(%f*%f)=%d\n", screen_width, screen_height, ratio, rows, ratio, rows, (int) round(rows * ratio));
  }
  //Check orientation and adjust accordingly
  if ( screen_height > screen_width )
  {
    return (int) round(rows);
  }
  else
  {
    return (int) round(rows * ratio);
  }
}

/**
* \brief
*/
static void parseAlignment(float *alignment_x, float *alignment_y, xmlChar** alignmentfromconf)
{
  char *result = NULL;
  
  result = strtok( *alignmentfromconf, "," );
  while( result != NULL ) {
    if ( strcmp(result, "top") == 0 )
    {
      *alignment_y = 0.0;
    }
    else if ( strcmp(result, "left") == 0 )
    {
      *alignment_x = 0.0;
    }
    else if ( strcmp(result, "bottom") == 0 )
    {
      *alignment_y = 1.0;
    }
    else if ( strcmp(result, "right") == 0 )
    {
      *alignment_x = 1.0;
    }
    else if ( strcmp(result, "center") == 0 )
    {
      /**
      * Only set the value if it has not been set previously
      * otherwise you might undo a previous parsed alignment
      * option
      */
      if (*alignment_y < 0.0)
      {
        *alignment_y = 0.5;
      }
      if (*alignment_x < 0.0)
      {
        *alignment_x = 0.5;
      }
    }
    if( DEBUG > 0 )
    {
      printf("parseAlignment: \"%s\" x:%f y:%f\n", result, *alignment_x, *alignment_y);
    }
    result = strtok( NULL, "," );
  }

  // Set initial values to 0
  if (*alignment_y < 0.0)
  {
    *alignment_y = 0.0;
  }
  if (*alignment_x < 0.0)
  {
    *alignment_x = 0.0;
  }
}

/**
* \brief Create a single button
* \param *elt pointer to menu_element struct that contains the logo image filename.
* \param max_width button width
*/
static GtkWidget* createbutton ( menu_elements *elt, int max_width, int max_height )
{
  GtkWidget *button, *logoimage;
  GdkPixbuf *pixbuf;
  int width, height;
  double ratio;
  
  if( elt->logo != NULL )
  {
    logoimage = gtk_image_new_from_file ((char *) elt->logo);
    pixbuf = gtk_image_get_pixbuf(GTK_IMAGE(logoimage));
    width = gdk_pixbuf_get_width(pixbuf);
    height = gdk_pixbuf_get_height(pixbuf);
    ratio = (double) width/max_width;
    width /= ratio;
    height /= ratio;

    // Check if button does not overlap the maximum allowed height
    if( height > max_height )
    {
      ratio = (double) height/max_height;
      width /= ratio;
      height /= ratio;
    }
    
    if (DEBUG > 0)
    {
      printf("createbutton:  button_width: %d button_height: %d ratio: %.4f\n", width, height, ratio);
    }
  
    pixbuf = gdk_pixbuf_scale_simple(pixbuf, width, height, GDK_INTERP_BILINEAR);
    gtk_image_set_from_pixbuf(GTK_IMAGE(logoimage), pixbuf);
  }
  
  button = gtk_button_new ();
  gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
  
  if( elt->logo != NULL )
  {
    gtk_button_set_image(GTK_BUTTON(button), logoimage);
  }
  
  gtk_button_set_focus_on_click(GTK_BUTTON(button), TRUE);

  // Signals to start the program
  g_signal_connect (G_OBJECT (button), "button_release_event", G_CALLBACK (process_startprogram_event), elt);
  g_signal_connect (G_OBJECT (button), "key_release_event", G_CALLBACK (process_startprogram_event), elt);

  // Signals to highlight the button
  g_signal_connect (G_OBJECT (button), "focus_in_event", G_CALLBACK (highlight_button), NULL);
  g_signal_connect (G_OBJECT (button), "focus_out_event", G_CALLBACK (dehighlight_button), NULL);
  g_signal_connect (G_OBJECT (button), "enter_notify_event", G_CALLBACK (highlight_button), NULL);
  g_signal_connect (G_OBJECT (button), "leave_notify_event", G_CALLBACK (dehighlight_button), NULL);

  // Signals to create button press effect when clicked
  g_signal_connect (G_OBJECT (button), "button_press_event", G_CALLBACK (press_button), elt);

  return button;
}



/**
* \brief Create the button layout using the available screen height and width
* \param *elts pointer to first menu_elements structure
* \param screen_height total screen height
* \param screen_width total screen width
*/
static GtkWidget* createbuttons( menu_elements *elts, int screen_width, int screen_height)
{
  menu_elements *next, *cur;
  GtkWidget* button, *hbox, *vbox, *align;
  struct appm_alignment *alignment;
  int elts_per_row, count, button_width;
  int box_width, box_height;
  float alignment_x = -1.0;
  float alignment_y = -1.0;
  
  box_width = calculateBoxLength(screen_width, elts->menu_width);
  box_height = calculateBoxLength(screen_height, elts->menu_height);

  if ( DEBUG > 0 )
  {
    printf("Creating button layout with screen width %d and screen height %d\n", screen_width, screen_height);
  }
  
  vbox = gtk_vbox_new (FALSE, 0);

  elts_per_row = calculateAmountOfElementsPerColumn(box_height, box_width, getNumberOfElements());
  if ( elts_per_row < 1 )
  {
    elts_per_row = 1;
  }

  button_width = box_width/elts_per_row;

  parseAlignment(&alignment_x, &alignment_y, (xmlChar **) elts->orientation);

  if (DEBUG > 0)
  {
    printf("createbuttons: box_height=%d, box_width=%d, numberElts=%d, elts_per_row=%d, button_width=%d\n", box_height, box_width, getNumberOfElements(), elts_per_row, button_width);
    printf("Alignment settings: gtk_alignment_new (%f, %f, %f, %f)\n", alignment_x, alignment_y, ((float) box_width/screen_width), ((float) box_height/screen_height));
    fflush(stdout);
  }

  cur=elts;
  count = 0;
  while(cur != NULL)
  {
    if( (count % elts_per_row) == 0 )
    {
      if (DEBUG > 0)
      {
        printf("Creating new row: %d\n", count % elts_per_row );
      }
      hbox = gtk_hbox_new (FALSE, 0);

      align = gtk_alignment_new (alignment_x, alignment_y, (float) box_width/screen_width, (float) box_height/screen_height);
      gtk_container_add (GTK_CONTAINER (align), hbox);
      gtk_widget_show (hbox);
      
      gtk_container_add (GTK_CONTAINER (vbox), align);
      gtk_widget_show (align);

    }

    next = cur->next;
    button = createbutton(cur, button_width, box_height);
    gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 1);
    gtk_widget_show (button);
    cur = next;
    count++;
  }
  
  align = gtk_alignment_new (alignment_x, alignment_y, (float) box_width/screen_width, (float) box_height/screen_height);

  gtk_container_add (GTK_CONTAINER (align), vbox);
  gtk_widget_show (align);
  gtk_widget_show (vbox);
  
  return align;
}

/**
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
    align = createbuttons( actions, screen_width, screen_height );
    gtk_container_add (GTK_CONTAINER (vbox), align);
    gtk_widget_show (align);
  }

  if ( programs != NULL )
  {
    align = createbuttons( programs, screen_width, screen_height );
    gtk_container_add (GTK_CONTAINER (vbox), align);
    gtk_widget_show (align);
  }

  gtk_container_add (GTK_CONTAINER (mainwin), vbox);
  gtk_widget_show (vbox);
  gtk_widget_show (mainwin);

  autostartprograms( programs );
  
  gtk_main ();

//   printf("Free programs\n");
//   fflush(stdout);
  freeMenuElements( programs );
//   printf("Free actions\n");
//   fflush(stdout);
  freeMenuElements( actions );
//   printf("Exit gappmanager\n");
//   fflush(stdout);
  return 0;
}

