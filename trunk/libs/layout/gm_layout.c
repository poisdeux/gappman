/**
 * \file gm_layout.c
 * \brief Main layout functions to create the user interface
 *
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 *
 * \todo Instead of specifying only max_elts add support to specify max buttonsize. Gappman would then determine based on the amount of buttons and buttonsize the amount of row and columns that are needed to create the menu_box.
 * \todo support larger sized framed buttons. Now the buttons are calculated using the image width and height. Some images occupy the complete button which makes it hard to see if they are highlighted.
 * \todo add support for different backgrounds in the menu's. This would make it possible to visually divide the UI.
 * \todo Add support for clutter (www.clutter-project.org) so we can have a fancy animated menu
 * \todo make gm_layout_create_menu generic for widgets and create a gm_layout_create_buttons that takes a menu struct and creates a list of GtkWidgets that can be passed to gm_layout_create_menu to create the menu.
 * \todo add support for i18n.
 */


#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <math.h>
#include <string.h>
#include <gm_generic.h>
#include <gm_layout.h>

#define MAXCHARSINLABEL 10		///< amount of characters we take as a
								// maximum to determine the fontsize.

#define FONTMETRIC 1024 ///< font metric is 1024th of a point.

static int window_width = 800;
static int window_height = 600;

static int g_fontsize = 10 * FONTMETRIC;	///< the default generic fontsize for all 
									// elements. This usually gets updated by
									// menu building functions below.

static void gm_layout_destroy_widget(GtkWidget * dummy, GdkEvent * event, GtkWidget * widget)
{
  if ( gm_layout_check_key(event) )
  {
    gtk_widget_destroy(widget);
  }
}

static void gm_layout_quit_program(GtkWidget * dummy, GdkEvent * event)
{
  // Only start program if spacebar or mousebutton is pressed
  if( gm_layout_check_key(event) )
  {
    gtk_main_quit();
  }
}

/**
* \brief Creates a box with an image and labeltext on the right of it.
* \param *elt menu_element for which the label must be created
* \param max_width maximum width for the box
* \param max_height maximum height for the box
* \return GtkWidget pointer to the hbox containing the label
*/
static GtkWidget *image_label_box_hor(gm_menu_element * elt, int max_width,
									  int max_height)
{
	GtkWidget *box;
	GtkWidget *label;
	GtkWidget *image;

	/* Create box for image and label */
	box = gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(box), 2);

	/* Now on to the image stuff */
	image =
		gm_layout_load_image((char *)elt->name, (char *)elt->logo,
					  (char *) gm_parseconf_get_cache_location(), (char *) gm_parseconf_get_programname(), max_width,
					  max_height);
	gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(image), FALSE, FALSE, 3);
	gtk_widget_show(GTK_WIDGET(image));

	if (elt->printlabel != 0)
	{
		/* Create a label for the button */
		label = gtk_label_new((const gchar *)elt->name);
		gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 3);
		gtk_widget_show(label);
	}

	return box;
}

/**
* \brief Creates a box with an image and labeltext below it.
* \param *elt menu_element for which the label must be created
* \param max_width maximum width for the box
* \param max_height maximum height for the box
* \return GtkWidget pointer to the hbox containing the label
*/
static GtkWidget *image_label_box_vert(gm_menu_element * elt, int max_width,
									   int max_height)
{
	GtkWidget *box;
	GtkWidget *label = NULL;
	GtkWidget *image;
	GtkRequisition requisition;

	/* Create box for image and label */
	box = gtk_vbox_new(FALSE, 0);

	if ((elt->printlabel != 0) && (elt->name != NULL))
	{
		label = gm_layout_create_label(elt->name);

		//obtain the size for label so we can account for it
		//when determining the image size
		gtk_widget_size_request(label, &requisition);
		image =
			gm_layout_load_image((char *)elt->name, (char *)elt->logo,
						(char*) gm_parseconf_get_cache_location(), (char*) gm_parseconf_get_programname(), max_width,
					  max_height - requisition.height);

		gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(image), TRUE, TRUE, 0);
		gtk_widget_show(image);

		gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 0);
		gtk_widget_show(label);
	
	}
	else
	{
		image =
			gm_layout_load_image((char *)elt->name, (char *)elt->logo,
						(char *)gm_parseconf_get_cache_location(), (char *)gm_parseconf_get_programname(), max_width,
					  max_height);
		gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(image), TRUE, TRUE, 0);
		gtk_widget_show(image);
	}

	return box;
}

/**
* \brief function to calculate the absolute width based upon the total available width
* \param total_length Total available width for the box element
* \param *box_length Pointer to a struct length holding the length value and type of the box
* \return box width in amount of pixels
*/
static int calculate_box_length(gint total_length, struct length *box_length)
{
	int length;

#ifdef DEBUG
g_debug("calculate_box_length: total_length=%d, box_length->value=%d", total_length, box_length->value);
#endif
	if (box_length->type == PERCENTAGE)
	{
		length =
			round((double)total_length * (box_length->value / (double)100));
	}
	else
	{
		length = box_length->value;
	}

	if (length > total_length)
	{
		g_warning
			("Box length exceeds total length. Check your configuration file.");
	}

#ifdef DEBUG
g_debug("calculate_box_length: total_length=%d, box_length->value=%d, length=%d", total_length, box_length->value, length);
#endif

	return length;
}

/**
* \brief function that highlights a button
* \param *widget pointer to the button widget
* \param *event the GdkEvent that occured.
* \param *elt will not be used, but must be defined.
*/
static gboolean press_button(GtkWidget * widget, GdkEvent * event)
{
	gtk_widget_activate(widget);
	return FALSE;
}

/**
* \brief function that highlights a button
* \param *widget pointer to the button widget
* \param *event the GdkEvent that occured.
* \param *elt will not be used, but must be defined.
*/
static gboolean highlight_button(GtkWidget * widget, GdkEvent * event)
{
	gtk_button_set_relief(GTK_BUTTON(widget), GTK_RELIEF_NORMAL);
	gtk_widget_grab_focus(widget);
	return FALSE;
}

/**
* \brief function that de-highlights a button
* \param *widget pointer to the button widget
* \param *event the GdkEvent that occured.
* \param *elt will not be used, but must be defined.
*/
static gboolean dehighlight_button(GtkWidget * widget, GdkEvent * event)
{
	gtk_button_set_relief(GTK_BUTTON(widget), GTK_RELIEF_NONE);
	return FALSE;
}

/**
* \brief Calculates the amount of elements per row to evenly spread all elements on a surface of box_height x box_width.
* \param box_width width in pixels of the box that should contain the elements.
* \param box_height height in pixels of the box that should contain the elements.
* \param amount_of_elements number of elements that should be placed in one row
*/
static int calculateAmountOfElementsPerRow(int box_width, int box_height,
											  int amount_of_elements)
{
	double elts;
	double ratio;
	
	if(box_height != 0 && box_width != 0)
	{
		ratio =
			(double)MAX(box_height, box_width) / (double)MIN(box_height,
														 box_width);
	}
	else
	{
		ratio = 1;
	}

	elts = sqrt(amount_of_elements / ratio);

	// Check orientation and adjust accordingly
	if (box_height > box_width)
	{
		return (int)round(elts);
	}
	else
	{
		return (int)round(elts * ratio);
	}
}

/**
* \brief scales an image to max_width unless that will make the button-heifght larger than max_height.
* \param image a pointer to a GtkWidget that holds the image
* \param max_width maximum allowed width of the button
* \param max_height maximum allowed height of the button
*/
static GdkPixbuf *scale_image(GtkWidget * image, int max_width, int max_height)
{
	GdkPixbuf *pixbuf;
	int width, height;
	gdouble ratio;

	if( max_width < 1 && max_height < 1 )
	{
		return NULL;
	}

	pixbuf = gtk_image_get_pixbuf(GTK_IMAGE(image));
	width = gdk_pixbuf_get_width(pixbuf);
	height = gdk_pixbuf_get_height(pixbuf);

	// By default we will scale the image to max_width maintaining aspect
	// ratio
	ratio = width / (gdouble) max_width;
	width /= ratio;
	height /= ratio;

	// Check if button does not overlap the maximum allowed height
	// if so we assume orientation is portrait and scale
	// button size based on max_height
	if (height > max_height)
	{
		width = gdk_pixbuf_get_width(pixbuf);
		height = gdk_pixbuf_get_height(pixbuf);

		ratio = height / (gdouble) max_height;
		width /= ratio;
		height /= ratio;
	}
	return gdk_pixbuf_scale_simple(pixbuf, width, height, GDK_INTERP_BILINEAR);
}

/**
* \brief creates a single menu page starting with menu elements from page_number. 
* The menu page will not hold more than menu->max_elts_in_single_box elements.
* \param menu pointer to gm_menu that holds the menu for which to create a page
* \param page_number when gm_menu holds more elements than menu->max_elts_in_single_box page_number specifies which page to create. Page count starts at 0. So for page 1 page_number should be equal to 0.
* \param widget_width preferred width for the widgets to include in the menu page
* \param widget_height preferred height for the widgets to include in the menu page
* \param elts_per_row maximum amount of menu elements in a single row
* \return GtkWidget pointer to a vbox
*/
static GtkWidget *create_menu_page_layout(gm_menu *menu, gint page_number, 
								gint elts_per_row)
{
	GtkWidget *button, *hbox, *vbox;
	gint menu_element_index;
  gint box_upper_limit;
	gm_menu_element *elt;

	if( ( elts_per_row < 1 ) || ( page_number < 0 ) )
	{
		g_warning("Cannot create menu with %d elements per row for page %d\n", elts_per_row, page_number);
		return NULL;
	}

	vbox = gtk_vbox_new(FALSE, 0);

	//calculate starting point in menu->elts array for the requested
  //page number
	menu_element_index = menu->max_elts_in_single_box * page_number;

	if( menu_element_index > menu->amount_of_elements )
	{
		g_warning("create_menu_page: no elements available to create page number: %d", page_number);
		return NULL;
	}

	if( ( menu_element_index + menu->max_elts_in_single_box ) < menu->amount_of_elements )
	{
		box_upper_limit = menu_element_index + menu->max_elts_in_single_box;
	}
	else
	{
		box_upper_limit = menu->amount_of_elements;
	}

	while(menu_element_index < box_upper_limit)
	{
		elt = menu->elts[menu_element_index];
		if (((menu_element_index) % elts_per_row) == 0)
		{
			hbox = gtk_hbox_new(FALSE, 0);

			gtk_container_add(GTK_CONTAINER(vbox), hbox);
		}

		if(elt->widget != NULL)
		{
			gtk_box_pack_start(GTK_BOX(hbox), elt->widget, TRUE, TRUE, 0);
			gtk_widget_set_size_request(button, menu->widget_width, menu->widget_height);
		}

		menu_element_index++;
	}

	return vbox;
}

static void switch_menu_left(GtkWidget *widget, GdkEvent *event, gm_menu *menu)
{
	int i;
	GtkWidget *tmp;

	if( menu->pages->prev == NULL )
		return;

	if( ! gm_layout_check_key(event) )
		return;

	//hide current box
	gtk_widget_hide(menu->pages->box);

	//always make sure menu->pages points to current
  //shown box
	menu->pages = menu->pages->prev;
	gtk_widget_show_all(menu->pages->box);
}

static void switch_menu_right(GtkWidget *widget, GdkEvent *event, gm_menu *menu)
{
	int i;
	GtkWidget *tmp;

	if( menu->pages->next == NULL )
		return;
	
	if( ! gm_layout_check_key(event) )
		return;

	//hide current box
	gtk_widget_hide(menu->pages->box);

	//always make sure menu->pages points to current
  //shown box
	menu->pages = menu->pages->next;
	gtk_widget_show_all(menu->pages->box);
}

gint gm_layout_calculate_fontsize(gchar *message)
{
	gint length;
	gint fontsize;

	//Use window width and height to calculate fontsize
  //is no message is provided
	if( message == NULL )
	{
    //Dividing by 50 gives 20pt font
    //for 1024 width. Empirically determined that
		//this provides a nice fontsize
		fontsize = round((double) window_width / (double) 50);	
	}
	else
	{
		length = strlen(message);

		if( length == 0 )
		{
			return 0;
		}	

		//calculate fontsize so message fits in window_width	
		fontsize = window_width / (double) length;

		
	}

	//Set fontsize to default 6pt if calculated 
 	//size is smaller than 6pt.	
	if ( fontsize < 6 )
	{ 
		fontsize = 6;
	}

	return fontsize * FONTMETRIC;
}

gboolean gm_layout_check_key(GdkEvent * event)
{
	// Only start program if spacebar or mousebutton is pressed
	if (((GdkEventKey *) event)->keyval == 32
		|| ((GdkEventButton *) event)->button == 1)
	{
		return TRUE;
	}

	return FALSE;
}

static void gm_layout_show_confirmation_dialog(gchar * message,
								 gchar * msg_button1, void *callback1,
								 void *data1, gchar * msg_button2,
								 void *callback2, void *data2,
								 GtkWidget * parent_window)
{
	GtkWidget *window;
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *button;
	GtkWidget *label;
	GdkPixbuf *pixbuf;
	GtkWidget *stock_image;
	gint fontsize;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

	gtk_widget_grab_focus(window);

	// Remove border
	gtk_window_set_decorated(GTK_WINDOW(window), FALSE);

	gtk_window_set_frame_dimensions(GTK_WINDOW(window), 5, 5, 5, 5);
	vbox = gtk_vbox_new(FALSE, 0);

	hbox = gtk_hbox_new(FALSE, 0);
	pixbuf =
		gtk_widget_render_icon(window, GTK_STOCK_DIALOG_QUESTION,
							   GTK_ICON_SIZE_DIALOG, NULL);
	stock_image = gtk_image_new_from_pixbuf(pixbuf);
	gtk_box_pack_start(GTK_BOX(hbox), stock_image, FALSE, FALSE, 0);
	gtk_widget_show(stock_image);

	fontsize = gm_layout_calculate_fontsize(message);
	gm_layout_set_fontsize(fontsize);	

	label = gm_layout_create_label(message);
	gtk_misc_set_padding(GTK_MISC(label), 5, 5);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	gtk_widget_show(label);

	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	gtk_widget_show(hbox);

	hbox = gtk_hbox_new(FALSE, 0);

	button = gm_layout_create_label_button(msg_button1, G_CALLBACK(callback1), data1);
  g_signal_connect(G_OBJECT(button), "button_release_event", G_CALLBACK(gm_layout_destroy_widget), window);
  g_signal_connect(G_OBJECT(button), "key_release_event", G_CALLBACK(gm_layout_destroy_widget), window);

	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
	gtk_widget_show(button);

	button = gm_layout_create_label_button(msg_button2, G_CALLBACK(callback2), data2);
  g_signal_connect(G_OBJECT(button), "button_release_event", G_CALLBACK(gm_layout_destroy_widget), window);
  g_signal_connect(G_OBJECT(button), "key_release_event", G_CALLBACK(gm_layout_destroy_widget), window);

	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
	gtk_widget_show(button);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	gtk_widget_show(hbox);

	gtk_container_add(GTK_CONTAINER(window), vbox);
	gtk_widget_show(vbox);
	gtk_widget_show(window);
}

void gm_layout_show_error_dialog(gchar * message, GtkWidget * parent_window,
						  void *callback)
{
	GtkWidget *window;
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *button;
	GtkWidget *label;
	GdkPixbuf *pixbuf;
	GtkWidget *stock_image;
	gint fontsize;

	g_warning("%s", message);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	//Check if caller wants dialog positioned 
  //relative to the parent window
	if (parent_window != NULL)
	{
		gtk_window_set_transient_for(GTK_WINDOW(window), GTK_WINDOW(parent_window));
		gtk_window_set_position(GTK_WINDOW(window),
								GTK_WIN_POS_CENTER_ON_PARENT);
	}
	else
	{
		gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	}

	// Remove border
	gtk_window_set_decorated(GTK_WINDOW(window), FALSE);

	gtk_window_set_frame_dimensions(GTK_WINDOW(window), 5, 5, 5, 5);
	vbox = gtk_vbox_new(FALSE, 0);

	hbox = gtk_hbox_new(FALSE, 0);
	pixbuf =
		gtk_widget_render_icon(window, GTK_STOCK_DIALOG_ERROR,
							   GTK_ICON_SIZE_DIALOG, NULL);
	stock_image = gtk_image_new_from_pixbuf(pixbuf);
	gtk_container_add(GTK_CONTAINER(hbox), stock_image);

	fontsize = gm_layout_calculate_fontsize(message);
	gm_layout_set_fontsize(fontsize);	

	label = gm_layout_create_label(message);
	gtk_misc_set_padding(GTK_MISC(label), 5, 5);
	gtk_container_add(GTK_CONTAINER(hbox), label);

	gtk_container_add(GTK_CONTAINER(vbox), hbox);

	button = gm_layout_create_label_button("Close", G_CALLBACK(callback), parent_window);
  g_signal_connect(G_OBJECT(button), "button_release_event", G_CALLBACK(gm_layout_destroy_widget), window);
  g_signal_connect(G_OBJECT(button), "key_release_event", G_CALLBACK(gm_layout_destroy_widget), window);

	gtk_container_add(GTK_CONTAINER(vbox), button);
	//gtk_widget_set_size_request(vbox, window_width, window_height);

	gtk_container_add(GTK_CONTAINER(window), vbox);
	//gtk_widget_set_size_request(window, window_width, window_height);
	gtk_widget_show_all(window);
}

void gm_layout_show_error(GmReturnCode code)
{
  switch (code)
  {
  case GM_NET_COMM_NOT_SUPPORTED:
    gm_layout_show_error_dialog("Gappman compiled without network support",
               NULL, NULL);
    break;;
  case GM_COULD_NOT_RESOLVE_HOSTNAME:
    gm_layout_show_error_dialog("Could not resolve hostname.",
               NULL, NULL);
    break;;
  case GM_COULD_NOT_CONNECT:
    gm_layout_show_error_dialog
      ("Could not connect to gappman.\nCheck that gappman is running.",
               NULL, NULL);
    break;;
  case GM_COULD_NOT_SEND_MESSAGE:
    gm_layout_show_error_dialog
      ("Could not sent message to localhost.\nCheck that gappman is running",
               NULL, NULL);
    break;;
  case GM_COULD_NOT_DISCONNECT:
    gm_layout_show_error_dialog("Could not disconnect from gappman.",
               NULL, NULL);
    break;;
  default:
    gm_layout_show_error_dialog
      ("An undefined error occured when contacting gappman.",
               NULL, NULL);
    break;;
  }
}

int gm_layout_get_fontsize()
{
	return g_fontsize;
}

void gm_layout_set_fontsize(gint size)
{
	g_fontsize = size;
}

void gm_layout_get_window_geometry(gint *width, gint *height)
{
	*width = window_width;
	*height = window_height;
}

void gm_layout_set_window_geometry(gint width, gint height)
{
	window_width = width;
	window_height = height;
}

GtkWidget *gm_layout_create_label(gchar *text)
{
	GtkWidget *label;
	gchar *markup;
	GtkRequisition requisition;
	int label_width, label_height;

	label = gtk_label_new("");
  markup = g_markup_printf_escaped("<span size=\"%d\">%s</span>", g_fontsize, text);
  gtk_label_set_markup(GTK_LABEL(label), markup);
  g_free(markup);
	gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);

	//make sure label is not larger then window
	gtk_widget_size_request(label, &requisition);

	label_width = ( requisition.width > window_width ) ? window_width : requisition.width;
	label_height = ( requisition.height > window_height ) ? window_height : requisition.height;

	gtk_widget_set_size_request(label, label_width, label_height);

#if defined(DEBUG)
g_debug("gm_layout_create_label: window: %dx%d, label: %dx%d", window_width, window_height, label_width, label_height);
#endif

	return label;
}

GtkWidget *gm_layout_create_label_button(gchar * buttontext, void *callbackfunc,
								  void *data)
{
	GtkWidget *button;
	GtkWidget *label;

#if defined(DEBUG)
g_debug("gm_layout_create_label_button: buttontext=%s", buttontext);
#endif

	label = gm_layout_create_label(buttontext);
	button = gm_layout_create_empty_button(callbackfunc, data);
	gtk_container_add(GTK_CONTAINER(button), label);
	gtk_widget_show(label);

	return button;
}

GtkWidget *gm_layout_load_image(gchar *elt_name, gchar *elt_logo,
						 gchar *cacheloc, gchar *programname,
						 gint max_width, gint max_height)
{
	GtkWidget *image = NULL;
	GdkPixbuf *pixbuf;
	char *cachedfile;
	size_t stringlength;
	FILE *fp;
	gchar *stock_id;
	GtkIconSize stock_size;


	if( elt_logo == NULL )
		return NULL;

	// Paths can be of arbitrary length.
	// Filenames can not be longer than 255 chars
	// Width and height will not exceed 9999 (4 chars)
	if (cacheloc != NULL)
	{
		stringlength = strlen(cacheloc) + 255 + 8;

		cachedfile = (char *)malloc(stringlength * sizeof(char) + 1);

		// Filename of cached image should conform to
		// CACHEDLOCATION/PROGRAMNAME-BUTTONNAME-WIDTHxHEIGHT
		sprintf(cachedfile, "%s/%s-%s-%dx%d", cacheloc, programname,
				elt_name, max_width, max_height);

		fp = fopen(cachedfile, "rw");
		if (fp)
		{
			image = gtk_image_new_from_file(cachedfile);
		}
		else
		{
			image = gtk_image_new_from_file((char *)elt_logo);
			if (gtk_image_get_storage_type(GTK_IMAGE(image)) ==
				GTK_IMAGE_STOCK)
			{
				gtk_image_get_stock(GTK_IMAGE(image), &stock_id, &stock_size);
				gtk_image_set_from_stock(GTK_IMAGE(image), stock_id,
										 stock_size);
			}
			else
			{
				pixbuf = scale_image(image, max_width, max_height);
				gtk_image_set_from_pixbuf(GTK_IMAGE(image), pixbuf);
				gdk_pixbuf_save(pixbuf, cachedfile, "png", NULL, "compression",
								"9", NULL);
			}
		}
		g_free(cachedfile);
	}
	else
	{
		image = gtk_image_new_from_file((char *)elt_logo);
		gtk_image_set_from_pixbuf(GTK_IMAGE(image),
								  scale_image(image, max_width, max_height));
	}
	return image;
}



GtkWidget *gm_layout_create_empty_button(void *callbackfunc, void *data)
{
	GtkWidget *button;

	button = gtk_button_new();
	gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);

	gtk_button_set_focus_on_click(GTK_BUTTON(button), TRUE);

	if (callbackfunc != NULL)
	{
		// Signals to start the program
		g_signal_connect(G_OBJECT(button), "button_release_event",
						 G_CALLBACK(callbackfunc), data);
		g_signal_connect(G_OBJECT(button), "key_release_event",
						 G_CALLBACK(callbackfunc), data);
	}

	// Signals to highlight the button
	g_signal_connect(G_OBJECT(button), "focus_in_event",
					 G_CALLBACK(highlight_button), NULL);
	g_signal_connect(G_OBJECT(button), "focus_out_event",
					 G_CALLBACK(dehighlight_button), NULL);
	g_signal_connect(G_OBJECT(button), "enter_notify_event",
					 G_CALLBACK(highlight_button), NULL);
	g_signal_connect(G_OBJECT(button), "leave_notify_event",
					 G_CALLBACK(dehighlight_button), NULL);

	// Signals to create button press effect when clicked
	g_signal_connect(G_OBJECT(button), "button_press_event",
					 G_CALLBACK(press_button), NULL);

	return button;
}

GtkWidget *gm_layout_create_button(gm_menu_element *elt, int max_width, int max_height,
							void (*processevent) (GtkWidget *, GdkEvent *,
												  gm_menu_element *))
{
	GtkWidget *button, *imagelabelbox;
	GtkBorder border;

#ifdef DEBUG
g_debug("gm_layout_create_button: elt->name=%s, max_width=%d, max_height=%d", elt->name, max_width, max_height);
g_assert(max_width <= window_width);
g_assert(max_height <= window_height);
#endif

	/**
	* Layout manager knows of three possible buttons:
  * 1. Logo + Label
  * 2. Label
  * 3. Logo
	*/

  //Check if none of the above is possible
	if( (elt->logo == NULL) && (elt->name == NULL) )
		return NULL;

	button = NULL;

	//Situation 1 + 3
	//image_label_box_vert determines if label should be included or not
	if( elt->logo != NULL )
	{
		button = gm_layout_create_empty_button(processevent, elt);
		imagelabelbox = image_label_box_vert(elt, max_width, max_height);
    gtk_container_add(GTK_CONTAINER(button), imagelabelbox);
    gtk_widget_show(imagelabelbox);
	}
	//Situation 2
	else if( elt->name != NULL )
	{
		button = gm_layout_create_label_button(elt->name, processevent, elt);
	}


	return button;
}

void gm_layout_calculate_sizes(gm_menu *menu)
{
	gint box_width;	
	gint box_height;	
	gint elts_per_row;
	gint elts_per_col;

	box_width = calculate_box_length(window_width, &(menu->menu_width));
	box_height = calculate_box_length(window_height, &(menu->menu_height));

	if ( menu->max_elts_in_single_box == 0 )
		menu->max_elts_in_single_box = menu->amount_of_elements;

	elts_per_row =
		calculateAmountOfElementsPerRow(box_width, box_height,
										   menu->max_elts_in_single_box);
	if (elts_per_row < 1)
	{
		elts_per_row = 1;
	}

	elts_per_col = menu->max_elts_in_single_box / elts_per_row;

	if (elts_per_col < 1)
	{
		elts_per_col = 1;
	}

	//calculate button geometry
	menu->widget_height = box_height / elts_per_col;
	if( menu->max_elts_in_single_box > menu->amount_of_elements )
	{
		//add room for page switchers
		menu->widget_width = (box_width*0.9) / elts_per_row;
	}
	else
	{
		menu->widget_width = box_width / elts_per_row;
	}

	menu->elts_per_row = elts_per_row;
	menu->box_width = box_width;
	menu->box_height = box_height;

#ifdef DEBUG
g_debug("calculate_sizes: menu->elts_per_row=%d, menu->box_width=%d, menu->box_height=%d, menu->widget_width=%d, menu->widget_height=%d, menu->max_elts_in_single_box=%d, menu->amount_of_elements=%d", menu->elts_per_row, menu->box_width, menu->box_height, menu->widget_width, menu->widget_height, menu->max_elts_in_single_box, menu->amount_of_elements);
#endif
}

GtkWidget *gm_layout_create_menu(gm_menu *menu)
{
	GtkWidget *fixed_box;
	GtkWidget *buttonbox;
	GtkWidget *hbox;
	GtkWidget *button;
	GtkWidget *tmp;
	gm_menu_page *page;
	gint box_width, box_height;
	gint elts_per_col;
	gint elts_per_row;
  gint button_height, button_width;
  gint number_of_pages;
	gint i;

#ifdef DEBUG
g_debug("gm_layout_create_menu: elts_per_row=%d, button_height=%d, button_width=%d, g_fontsize=%d", menu->elts_per_row, menu->widget_height, menu->widget_width, g_fontsize);
#endif

  hbox = gtk_hbox_new(FALSE, 0);

	if( menu->max_elts_in_single_box == 0 )
	{
		menu->max_elts_in_single_box = menu->amount_of_elements;
	}

	//calculate needed pages rounding a double to its smallest integer value that is not less
  //than its double value
	number_of_pages = ceil(menu->amount_of_elements / (double) menu->max_elts_in_single_box);

	elts_per_row = menu->elts_per_row;

  for(i = 0;i < number_of_pages; i++)
  {
    buttonbox = create_menu_page_layout(menu, i, elts_per_row);
		page = gm_menu_page_create(buttonbox);
		if( gm_menu_add_page(page, menu) == GM_FAIL )
			g_warning("gm_layout_create_menu: failed to add page");

    gtk_widget_hide_all(buttonbox);
  }

	box_width = menu->box_width;
	box_height = menu->box_height;

	//check if we got more than one buttonbox in the menu
	//if so we add arrow keys to switch pages
	if(menu->pages->prev != NULL)
	{
		//add the left arrowbutton
		fixed_box = gtk_fixed_new();
		button = gm_layout_create_empty_button(switch_menu_left, menu);
		gtk_widget_set_size_request(button, box_width*0.05, box_height);
		gtk_fixed_put(GTK_FIXED(fixed_box), button, 0, 0);
		gtk_widget_show(button);

		gtk_fixed_put(GTK_FIXED(fixed_box), buttonbox, box_width*0.05, 0);
		gtk_widget_show(buttonbox);

		//add the right arrowbutton
		button = gm_layout_create_empty_button(switch_menu_right, menu);
		gtk_widget_set_size_request(button, box_width*0.05, box_height);
		gtk_fixed_put(GTK_FIXED(fixed_box), button, box_width-(box_width*0.05), 0);
		gtk_widget_show(button);
	}
	else
	{
		gtk_container_add(GTK_CONTAINER(hbox), buttonbox);
	}

	gtk_widget_show_all(menu->pages->box);
	return hbox;
}

/* TO BE IMPLEMENTED */
GtkWidget *gm_layout_create_box(GtkWidget **widgets)
{
	return NULL;
}


