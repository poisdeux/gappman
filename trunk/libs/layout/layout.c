#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <../parseconf/parseconf.h>
#include <math.h>
#include <string.h>

static int DEBUG = 0;

/**
* \brief callback function to quit the program
* \param *widget pointer to widget to destroy
* \param data mandatory argument for callback function, may be NULL.
*/
static void layout_destroy( GtkWidget *widget,
                     gpointer   data )
{
    gtk_main_quit ();
}

static GdkPixbuf* scale_image(GtkWidget *image, int max_width, int max_height)
{
  GdkPixbuf* pixbuf;
  int width, height;
  double ratio;

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
  return gdk_pixbuf_scale_simple(pixbuf, width, height, GDK_INTERP_BILINEAR);
}
 
/**
* \brief loads image and scales it making sure the image fits inside
* max_width*max_height maintaining the correct aspect ratio
* \param imagefile filename of the image on disk
* \param max_width maximum width image may have
* \param max_height maximum height image may have
* \return GtkWidget pointer to image
*/
GtkWidget* load_image(menu_elements *elt, int max_width, int max_height)
{
  GtkWidget *image;
  GdkPixbuf *pixbuf;
  char* cachedfile;
  size_t stringlength;
  FILE *fp;

  // Paths can be of arbitrary lengt.
  // Filenames can not be longer than 255 chars
  // Width and height will not exceed 9999 (4 chars)
  if ( getCachelocation() != NULL )
  {
    stringlength = strlen(getCachelocation()) + 255 + 8;

    cachedfile = (char*) malloc(stringlength * sizeof(char) + 1);
  
    //Filename of cached image should conform to
    //CACHEDLOCATION/PROGRAMNAME-BUTTONNAME-WIDTHxHEIGHT
    sprintf(cachedfile, "%s/%s-%s-%dx%d", getCachelocation(), getProgramname(), 
    	elt->name, max_width, max_height);
    //printf("DEBUG: CACHEDFILE: %s\n", cachedfile);
    fflush(stdout);

    fp = fopen(cachedfile, "rw");
    if ( fp )
    {
	//printf("DEBUG: Reading cachefile\n");
        image = gtk_image_new_from_file (cachedfile);
    }
    else
    {
	//printf("DEBUG: Creating cachefile\n");
        image = gtk_image_new_from_file ((char*) elt->logo);
	pixbuf = scale_image(image, max_width, max_height); 
        gtk_image_set_from_pixbuf(GTK_IMAGE(image), pixbuf);
	gdk_pixbuf_save(pixbuf, cachedfile, "png", NULL, "compression", "9", NULL);	
    }

    free(cachedfile);
  }
  else
  {
    image = gtk_image_new_from_file ((char*) elt->logo);
    gtk_image_set_from_pixbuf(GTK_IMAGE(image), scale_image(image, max_width, max_height));
  }
  
  return image;
}

/**
* \brief Creates a box with an image and labeltext on the right of it.
* \param imagefile filename of image on disk
* \param labeltext string containing label
* \param max_width maximum allowed width the image may have
* \param max_height maximum allowed height the image may have 
* \return GtkWidget pointer 
*/
GtkWidget* image_label_box_hor (menu_elements *elt, gchar* labeltext, int max_width, int max_height)
{
    GtkWidget *box;
    GtkWidget *label;
    GtkWidget *image;

    /* Create box for image and label */
    box = gtk_hbox_new (FALSE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (box), 2);

    /* Now on to the image stuff */
    image = load_image(elt, max_width, max_height);
    gtk_box_pack_start (GTK_BOX (box), image, FALSE, FALSE, 3);
    gtk_widget_show (image);

    if(labeltext != NULL)
    { 
      /* Create a label for the button */
      label = gtk_label_new (labeltext);
      gtk_box_pack_start (GTK_BOX (box), label, FALSE, FALSE, 3);
      gtk_widget_show (label);
    }

    return box;
}

/**
* \brief Creates a box with an image and labeltext below it.
* \param imagefile filename of image on disk
* \param labeltext string containing label
* \param max_width maximum allowed width the image may have
* \param max_height maximum allowed height the image may have  
*/
GtkWidget* image_label_box_vert (menu_elements *elt, gchar* labeltext, int fontsize, int max_width, int max_height)
{
    GtkWidget *box;
    GtkWidget *label;
    GtkWidget *image;
		gchar *markup;

    /* Create box for image and label */
    box = gtk_vbox_new (FALSE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (box), 2);

    /* Now on to the image stuff */
    image = load_image(elt, max_width, max_height);
    gtk_box_pack_start (GTK_BOX (box), image, FALSE, FALSE, 3);
    gtk_widget_show (image);

    if(labeltext != NULL)
    { 
      label = gtk_label_new ("");
			if(fontsize != -1)
			{
				markup = g_markup_printf_escaped ("<span size=\"%d\">%s</span>", fontsize, labeltext);
  			gtk_label_set_markup (GTK_LABEL (label), markup);
		  	g_free (markup);
			}
      gtk_box_pack_start (GTK_BOX (box), label, FALSE, FALSE, 3);
      gtk_widget_show (label);
    }

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
static void parseAlignment(float *alignment_x, float *alignment_y, char** alignmentfromconf)
{
	int i;

	i = 0;	
  while( alignmentfromconf[i] != NULL ) {
    if ( strcmp(alignmentfromconf[i], "top") == 0 )
    {
      *alignment_y = 0.0;
    }
    else if ( strcmp(alignmentfromconf[i], "left") == 0 )
    {
      *alignment_x = 0.0;
    }
    else if ( strcmp(alignmentfromconf[i], "bottom") == 0 )
    {
      *alignment_y = 1.0;
    }
    else if ( strcmp(alignmentfromconf[i], "right") == 0 )
    {
      *alignment_x = 1.0;
    }
    else if ( strcmp(alignmentfromconf[i], "center") == 0 )
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
      printf("parseAlignment: \"%s\" x:%f y:%f\n", alignmentfromconf[i], *alignment_x, *alignment_y);
    }
		i = i + 1;
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
GtkWidget* createbutton ( menu_elements *elt, int fontsize, int max_width, int max_height, gboolean (*processevent)(GtkWidget*, GdkEvent*, menu_elements*) )
{
  GtkWidget *button, *imagelabelbox;
  GdkPixbuf *pixbuf;
  int width, height;
  double ratio;

  if( elt->logo != NULL )
  {
    if(elt->printlabel == 0)
    {
      //NO LABEL THANK YOU VERY MUCH
      imagelabelbox = image_label_box_vert(elt , NULL, fontsize, max_width, max_height);
    }
    else
    {
      //LABEL PLEASE
      imagelabelbox = image_label_box_vert(elt , (gchar*) elt->name, fontsize, max_width, max_height);
    }
    if (DEBUG > 0)
    {
      printf("createbutton:  button_width: %d button_height: %d ratio: %.4f\n", width, height, ratio);
    }
  }

  button = gtk_button_new ();
  gtk_container_add(GTK_CONTAINER(button), imagelabelbox);
  gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);

  if( elt->logo != NULL )
  {
    gtk_button_set_image(GTK_BUTTON(button), imagelabelbox);
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

  parseAlignment(&alignment_x, &alignment_y, elts->orientation);

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
    button = createbutton(cur, -1, button_width, box_height, processevent);
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


