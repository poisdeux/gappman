#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <../parseconf/parseconf.h>
#include <math.h>
#include <string.h>

static int DEBUG = 0;

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
* \brief Calculates the amount of elements per row to evenly spread all elements on a surface of screen_heightx screen_width.
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
static GtkWidget* createbutton ( menu_elements *elt, int max_width, int max_height, gboolean (*processevent)(GtkWidget*, GdkEvent*, menu_elements*) )
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
  // process_startprogram_event must be replaced with argument passed to this function?!?
  g_signal_connect (G_OBJECT (button), "button_release_event", G_CALLBACK (processevent), elt);
  g_signal_connect (G_OBJECT (button), "key_release_event", G_CALLBACK (processevent), elt);

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
GtkWidget* createbuttons( menu_elements *elts, int screen_width, int screen_height, gboolean(processevent)(GtkWidget*, GdkEvent*, menu_elements*))
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
    button = createbutton(cur, button_width, box_height, processevent);
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


