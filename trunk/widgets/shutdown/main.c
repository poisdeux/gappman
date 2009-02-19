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
#include <../../libs/parseconf/parseconf.h>

static int DEBUG = 0;
static int WINDOWED = 0;

static void usage()
{
  printf("usage: shutdown [--help] [--debug <LEVEL>] [--width <WIDTHINPIXELS>] [--height <HEIGHTINPIXELS>] [--conffile <FILENAME>] [--gtkrc <GTKRCFILENAME>] [--windowed]\n");
  printf("");
  printf("--help:\t\tshows this help text\n");
  printf("--debug <LEVEL>:\t\tsets verbosity leven\n");
  printf("--width <WIDTHINPIXELS>:\t\twidth of the main window (default: screen width / 9)\n");
  printf("--height <HEIGHTINPIXELS:\t\theight of the main window (default: screen height / 9)\n");
  printf("--conffile <FILENAME>:\t\t configuration file specifying the program and actions (default: /etc/gappman/shutdown.xml)\n");
  printf("--gtkrc <GTKRCFILENAME>:\t\t gtk configuration file which can be used for themeing\n");
  printf("--windowed:\t\t creates a border around the window\n");

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
    //startprogram( widget, elt );
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
static int calculateAmountOfElementsPerColumn(int height, int width, int amount_of_elements)
{
  double rows;
  double ratio;
  ratio = (double) MAX(height, width) / (double) MIN(height, width);
  rows = sqrt(amount_of_elements/ratio);

  if (DEBUG > 0)
  {
    printf("calculateAmountOfElementsPerColumn: width=%d, height=%d, ratio=%f, rows=%f, round(%f*%f)=%d\n", width, height, ratio, rows, ratio, rows, (int) round(rows * ratio));
  }
  //Check orientation and adjust accordingly
  if ( height > width )
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

static GtkWidget* load_image(char* imagefile, int max_width, int max_height)
{
  GtkWidget *image;
  GdkPixbuf *pixbuf;
  int width,height;
  double ratio;

  image = gtk_image_new_from_file (imagefile);
  pixbuf = gtk_image_get_pixbuf(GTK_IMAGE(image));
  width = gdk_pixbuf_get_width(pixbuf);
  height = gdk_pixbuf_get_height(pixbuf);
  ratio = (double) width/max_width;
  width /= ratio;
  height /= ratio;

  // Check if button does not overlap the maximum allowed height
  // if so we assume orientation is portrait and determine 
  // button size based on height
  if( height > max_height )
  {
    ratio = (double) height/max_height;
    width /= ratio;
    height /= ratio;
  }
  pixbuf = gdk_pixbuf_scale_simple(pixbuf, width, height, GDK_INTERP_BILINEAR);
  gtk_image_set_from_pixbuf(GTK_IMAGE(image), pixbuf);

  return image;
}

static GtkWidget* image_label_box (gchar* imagefile, gchar* labeltext, int max_width, int max_height)
{
    GtkWidget *box;
    GtkWidget *label;
    GtkWidget *image;

    /* Create box for image and label */
    box = gtk_vbox_new (FALSE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (box), 2);

    /* Now on to the image stuff */
    image = load_image(imagefile, max_width, max_height);

    /* Create a label for the button */
    label = gtk_label_new (labeltext);

    /* Pack the image and label into the box */
    gtk_box_pack_start (GTK_BOX (box), image, FALSE, FALSE, 3);
    gtk_box_pack_start (GTK_BOX (box), label, FALSE, FALSE, 3);

    gtk_widget_show (image);
    gtk_widget_show (label);

    return box;
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

/**
* \brief Create the button layout using the available screen height and width
* \param *elts pointer to first menu_elements structure
* \param screen_height total screen height
* \param screen_width total screen width
*/
static GtkWidget* createbuttons( menu_elements *elts, int dialog_width, int dialog_height)
{
  menu_elements *next, *cur;
  GtkWidget* button, *hbox, *vbox, *align;
  struct appm_alignment *alignment;
  int elts_per_row, count, button_width;
  int box_width, box_height;
  float alignment_x = -1.0;
  float alignment_y = -1.0;

  if ( DEBUG > 0 )
  {
    printf("Creating dialog layout with width %d and height %d\n", dialog_width, dialog_height);
  }

  vbox = gtk_vbox_new (FALSE, 0);

  elts_per_row = calculateAmountOfElementsPerColumn(dialog_height, dialog_width, getNumberOfElements());
  if ( elts_per_row < 1 )
  {
    elts_per_row = 1;
  }

  button_width = dialog_width/elts_per_row;

  parseAlignment(&alignment_x, &alignment_y, (xmlChar **) elts->orientation);

  if (DEBUG > 0)
  {
    printf("createbuttons: box_height=%d, box_width=%d, numberElts=%d, elts_per_row=%d, button_width=%d\n", box_height, box_width, getNumberOfElements(), elts_per_row, button_width);
    printf("Alignment settings: gtk_alignment_new (%f, %f, %f, %f)\n", alignment_x, alignment_y, ((float) box_width/dialog_width), ((float) box_height/dialog_height));
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

      align = gtk_alignment_new (alignment_x, alignment_y, (float) dialog_width, (float) dialog_height);
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

  align = gtk_alignment_new (alignment_x, alignment_y, (float) box_width/dialog_width, (float) box_height/dialog_height);

  gtk_container_add (GTK_CONTAINER (align), vbox);
  gtk_widget_show (align);
  gtk_widget_show (vbox);

  return align;
}


/**
* \brief main function setting up the UI
*/
int main (int argc, char **argv)
{
  GdkScreen *screen;
  GdkWindow *rootwin;
  GtkWidget *button;
  GtkWidget *labelimagebox;
  GtkWidget *mainwin;
  GtkWidget *table;
  menu_elements *actions;
  const char* conffile = "/etc/gappman/shutdown.xml";
  int dialog_width;
  int dialog_height;
  int c;
  char* shutdownimagefile = "/usr/share/pixmaps/gappman/system-shutdown.png";
  char* restartimagefile = "/usr/share/pixmaps/gappman/system-restart.png";
  char* suspendimagefile = "/usr/share/pixmaps/gappman/system-suspend.png";

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
          {"windowed", 0, 0, 'j'},
          {0, 0, 0, 0}
      };
      c = getopt_long(argc, argv, "w:h:c:d:r:ij",
              long_options, &option_index);
      if (c == -1)
          break;

      switch (c) {
      case 'w':
          dialog_width=atoi(optarg);
          break;
      case 'h':
          dialog_height=atoi(optarg);
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
  //actions = getActions();
  
  screen = gdk_screen_get_default ();
  dialog_width =  gdk_screen_get_width (screen) / 9;
  dialog_height =  gdk_screen_get_height (screen) / 9;

  mainwin = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  
  //Make window transparent
  //  gtk_window_set_opacity (GTK_WINDOW (mainwin), 0.0);
  
  //Remove border
  gtk_window_set_decorated (GTK_WINDOW (mainwin), FALSE);
  gtk_window_set_default_size (GTK_WINDOW (mainwin), dialog_width, dialog_height);

  // shutdown, reboot, suspend
  table = gtk_table_new(2, 3, TRUE);

  // shutdown button
  button = gtk_button_new();
  labelimagebox = image_label_box("/usr/share/pixmaps/gappman/system-shutdown.png", "Shutdown", dialog_width, dialog_height); 
  gtk_container_add (GTK_CONTAINER (button), labelimagebox);
  gtk_widget_show (labelimagebox);
  gtk_widget_show(button);
  gtk_table_attach_defaults(GTK_TABLE(table), button, 0, 1, 0, 1); 
  
  // reboot button
  button = gtk_button_new();
  labelimagebox = image_label_box("/usr/share/pixmaps/gappman/system-restart.png", "Reboot", dialog_width, dialog_height); 
  gtk_container_add (GTK_CONTAINER (button), labelimagebox);
  gtk_widget_show (labelimagebox);
  gtk_widget_show(button);
  gtk_table_attach_defaults(GTK_TABLE(table), button, 1, 2, 0, 1); 

  // suspend button
  button = gtk_button_new();
  labelimagebox = image_label_box("/usr/share/pixmaps/gappman/system-suspend.png", "Suspend", dialog_width, dialog_height); 
  gtk_container_add (GTK_CONTAINER (button), labelimagebox);
  gtk_widget_show (labelimagebox);
  gtk_widget_show(button);
  gtk_table_attach_defaults(GTK_TABLE(table), button, 2, 3, 0, 1); 
 
  // cancel button
  button = gtk_button_new_with_label("Cancel");
  gtk_widget_show(button);
  gtk_table_attach_defaults(GTK_TABLE(table), button, 1, 3, 1, 2); 
  
  gtk_container_add (GTK_CONTAINER (mainwin), table);
  gtk_widget_show (table);
  gtk_widget_show (mainwin);
  
  gtk_main ();

  return 0;
}

