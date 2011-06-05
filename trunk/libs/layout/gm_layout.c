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
 *  \todo Add support for multiple program-menu's. 
 *  Implementation: Add max_elts attribute to configuration file. In gm_create_buttonbox we
 *  will need to check if elts->numberElts > max_elts and if so 
 *  add arrow buttons left and right of the box. These buttons would allow the user to switch
 *  between different program-menu's.
 */


#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <math.h>
#include <string.h>
#include <gm_layout.h>
#include <gm_generic.h>
#include <gm_parseconf.h>

static int fontsize = 10 * 1024;	///< the default generic fontsize for all 
									// elements. This usually gets updated by
									// menu building functions below.
static int screen_width = 800;
static int screen_height = 600;

#define MAXCHARSINLABEL 15;		///< amount of characters we take as a
								// maximum to determine the fontsize.

gboolean check_key(GdkEvent * event)
{
	// Only start program if spacebar or mousebutton is pressed
	if (((GdkEventKey *) event)->keyval == 32
		|| ((GdkEventButton *) event)->button == 1)
	{
		return TRUE;
	}

	return FALSE;
}

static void destroy_widget(GtkWidget * widget, gpointer data)
{
	gtk_widget_destroy(widget);
}

void gm_destroy_widget(GtkWidget * dummy, GdkEvent * event, GtkWidget * widget)
{
	// Only start program if spacebar or mousebutton is pressed
	if (((GdkEventKey *) event)->keyval == 32
		|| ((GdkEventButton *) event)->button == 1)
	{
		gtk_widget_destroy(widget);
	}
}

void gm_quit_program(GtkWidget * dummy, GdkEvent * event)
{
	// Only start program if spacebar or mousebutton is pressed
	if (((GdkEventKey *) event)->keyval == 32
		|| ((GdkEventButton *) event)->button == 1)
	{
		gtk_main_quit();
	}
}

void gm_show_confirmation_dialog(const gchar * message,
								 const gchar * msg_button1, void *callback1,
								 void *data1, const gchar * msg_button2,
								 void *callback2, void *data2,
								 GtkWidget * mainwin)
{
	GtkWidget *window;
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *button;
	GtkWidget *label;
	gchar *markup;
	GdkPixbuf *pixbuf;
	GtkWidget *stock_image;

	g_warning("%s", message);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	if (mainwin != NULL)
	{
		gtk_window_set_transient_for(GTK_WINDOW(window), GTK_WINDOW(mainwin));
		gtk_window_set_position(GTK_WINDOW(window),
								GTK_WIN_POS_CENTER_ON_PARENT);
	}
	else
	{
		gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	}

	gtk_widget_grab_focus(window);

	// Make window transparent
	// gtk_window_set_opacity (GTK_WINDOW (window), 0.8);

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

	label = gtk_label_new("");
	markup =
		g_markup_printf_escaped("<span size=\"%d\">%s</span>", fontsize,
								message);
	gtk_label_set_markup(GTK_LABEL(label), markup);
	g_free(markup);
	gtk_misc_set_padding(GTK_MISC(label), 5, 5);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	gtk_widget_show(label);

	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	gtk_widget_show(hbox);

	hbox = gtk_hbox_new(FALSE, 0);
	label = gtk_label_new("");
	markup =
		g_markup_printf_escaped("<span size=\"%d\">%s</span>", fontsize,
								msg_button1);
	gtk_label_set_markup(GTK_LABEL(label), markup);
	g_free(markup);
	button = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(button), label);
	gtk_widget_show(label);
	if (callback1 != NULL)
	{
		g_signal_connect_swapped(G_OBJECT(button), "clicked",
								 G_CALLBACK(callback1), data1);
	}
	g_signal_connect_swapped(G_OBJECT(button), "clicked",
							 G_CALLBACK(destroy_widget), window);

	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
	gtk_widget_show(button);

	label = gtk_label_new("");
	markup =
		g_markup_printf_escaped("<span size=\"%d\">%s</span>", fontsize,
								msg_button2);
	gtk_label_set_markup(GTK_LABEL(label), markup);
	g_free(markup);
	button = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(button), label);
	gtk_widget_show(label);
	g_signal_connect_swapped(G_OBJECT(button), "clicked",
							 G_CALLBACK(destroy_widget), window);
	if (callback2 != NULL)
	{
		g_signal_connect_swapped(G_OBJECT(button), "clicked",
								 G_CALLBACK(callback2), data2);
	}
	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
	gtk_widget_show(button);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	gtk_widget_show(hbox);

	gtk_container_add(GTK_CONTAINER(window), vbox);
	gtk_widget_show(vbox);
	gtk_widget_show(window);
}

void gm_show_error_dialog(const gchar * message, GtkWidget * mainwin,
						  void *callback)
{
	GtkWidget *window;
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *button;
	GtkWidget *label;
	gchar *markup;
	GdkPixbuf *pixbuf;
	GtkWidget *stock_image;

	g_warning("%s", message);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	if (mainwin != NULL)
	{
		gtk_window_set_transient_for(GTK_WINDOW(window), GTK_WINDOW(mainwin));
		gtk_window_set_position(GTK_WINDOW(window),
								GTK_WIN_POS_CENTER_ON_PARENT);
	}
	else
	{
		gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	}

	// Make window transparent
	// gtk_window_set_opacity (GTK_WINDOW (window), 0.8);

	// Remove border
	gtk_window_set_decorated(GTK_WINDOW(window), FALSE);

	gtk_window_set_frame_dimensions(GTK_WINDOW(window), 5, 5, 5, 5);
	vbox = gtk_vbox_new(FALSE, 0);

	hbox = gtk_hbox_new(FALSE, 0);
	pixbuf =
		gtk_widget_render_icon(window, GTK_STOCK_DIALOG_ERROR,
							   GTK_ICON_SIZE_DIALOG, NULL);
	stock_image = gtk_image_new_from_pixbuf(pixbuf);
	gtk_box_pack_start(GTK_BOX(hbox), stock_image, FALSE, FALSE, 0);
	gtk_widget_show(stock_image);

	label = gtk_label_new("");
	markup =
		g_markup_printf_escaped("<span size=\"%d\">%s</span>", fontsize,
								message);
	gtk_label_set_markup(GTK_LABEL(label), markup);
	g_free(markup);
	gtk_misc_set_padding(GTK_MISC(label), 5, 5);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	gtk_widget_show(label);

	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	gtk_widget_show(hbox);

	label = gtk_label_new("");
	markup =
		g_markup_printf_escaped("<span size=\"%d\">Close</span>", fontsize);
	gtk_label_set_markup(GTK_LABEL(label), markup);
	g_free(markup);
	button = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(button), label);
	gtk_widget_show(label);
	if (callback != NULL)
	{
		g_signal_connect_swapped(G_OBJECT(button), "clicked",
								 G_CALLBACK(callback), mainwin);
	}
	else
	{
		g_signal_connect_swapped(G_OBJECT(button), "clicked",
								 G_CALLBACK(destroy_widget), window);
	}
	gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
	gtk_widget_show(button);

	gtk_container_add(GTK_CONTAINER(window), vbox);
	gtk_widget_show(vbox);
	gtk_widget_show(window);
}

int gm_get_fontsize()
{
	return fontsize;
}

void gm_set_fontsize(int size)
{
	fontsize = size;
}

void gm_set_window_geometry(int width, int height)
{
	screen_width = width;
	screen_height = height;
}

GtkWidget *gm_create_label_button(gchar * buttontext, void *callbackfunc,
								  void *data)
{
	GtkWidget *button;
	GtkWidget *label;
	gchar *markup;

	label = gtk_label_new("");
	markup =
		g_markup_printf_escaped("<span size=\"%d\">%s</span>", fontsize,
								buttontext);
	gtk_label_set_markup(GTK_LABEL(label), markup);
	g_free(markup);
	button = gm_create_empty_button(callbackfunc, data);
	gtk_container_add(GTK_CONTAINER(button), label);
	gtk_widget_show(label);

	return button;
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

GtkWidget *gm_load_image(const char *elt_name, const char *elt_logo,
						 const char *cacheloc, const char *programname,
						 int max_width, int max_height)
{
	GtkWidget *image = NULL;
	GdkPixbuf *pixbuf;
	char *cachedfile;
	size_t stringlength;
	FILE *fp;
	gchar *stock_id;
	GtkIconSize stock_size;


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
		free(cachedfile);
	}
	else
	{
		image = gtk_image_new_from_file((char *)elt_logo);
		gtk_image_set_from_pixbuf(GTK_IMAGE(image),
								  scale_image(image, max_width, max_height));
	}
	return image;
}

/**
* \brief Creates a box with an image and labeltext on the right of it.
* \param *elt menu_element for which the label must be created
* \param max_width maximum width for the box
* \param max_height maximum height for the box
* \return GtkWidget pointer to the hbox containing the label
*/
static GtkWidget *image_label_box_hor(menu_elements * elt, int max_width,
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
		gm_load_image((char *)elt->name, (char *)elt->logo,
					  gm_get_cache_location(), gm_get_programname(), max_width,
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
static GtkWidget *image_label_box_vert(menu_elements * elt, int max_width,
									   int max_height)
{
	GtkWidget *box;
	GtkWidget *label = NULL;
	GtkWidget *image;
	GtkRequisition requisition;
	gchar *markup;

	/* Create box for image and label */
	box = gtk_vbox_new(FALSE, 0);

	if ((elt->printlabel != 0) && (elt->name != NULL))
	{
		label = gtk_label_new("");
		markup =
			g_markup_printf_escaped("<span size=\"%d\">%s</span>", fontsize,
									elt->name);
		gtk_label_set_markup(GTK_LABEL(label), markup);
		g_free(markup);

		//obtain the size for label so we can account for it
		//when determining the image size
		gtk_widget_size_request(label, &requisition);
		image =
			gm_load_image((char *)elt->name, (char *)elt->logo,
						gm_get_cache_location(), gm_get_programname(), max_width,
					  max_height - requisition.height);

		gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(image), TRUE, TRUE, 0);
		gtk_widget_show(image);

		gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 0);
		gtk_widget_show(label);
	
	}
	else
	{
		image =
			gm_load_image((char *)elt->name, (char *)elt->logo,
						gm_get_cache_location(), gm_get_programname(), max_width,
					  max_height);
		gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(image), TRUE, TRUE, 0);
		gtk_widget_show(image);
	}

	return box;
}

int gm_calculate_box_length(int total_length, struct length *box_length)
{
	int length;

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
			("Box length exceeds total length. This might indicate a configuration error.");
	}

	return length;
}

/**
* \brief function that highlights a button
* \param *widget pointer to the button widget
* \param *event the GdkEvent that occured.
* \param *elt will not be used, but must be defined.
*/
static gboolean press_button(GtkWidget * widget, GdkEvent * event,
							 menu_elements * elt)
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
static gboolean highlight_button(GtkWidget * widget, GdkEvent * event,
								 menu_elements * elt)
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
static gboolean dehighlight_button(GtkWidget * widget, GdkEvent * event,
								   menu_elements * elt)
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
	ratio =
		(double)MAX(box_height, box_width) / (double)MIN(box_height,
														 box_width);
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

GtkWidget *gm_create_empty_button(void *callbackfunc, void *data)
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

GtkWidget *gm_create_button(menu_elements * elt, int max_width, int max_height,
							void (*processevent) (GtkWidget *, GdkEvent *,
												  menu_elements *))
{
	GtkWidget *button, *imagelabelbox;
	GtkBorder border;

	button = gm_create_empty_button(processevent, elt);
	gtk_widget_set_size_request(button, max_width, max_height);
	if (elt->logo != NULL)
	{
		imagelabelbox = image_label_box_vert(elt, max_width, max_height);
		gtk_container_add(GTK_CONTAINER(button), imagelabelbox);
		gtk_widget_show(imagelabelbox);
	}

	return button;
}

/**
* \brief Create a single button
* \param *elt pointer to menu_element struct that contains the logo image filename.
* \param width button width
* \param height button height
*/
static GtkWidget *createpanelelement(menu_elements * elt, int width,
									 int height)
{
	GModule *module;

	if (!g_module_supported())
	{
		return NULL;
	}

	module = g_module_open((const gchar *)elt->module, G_MODULE_BIND_LAZY);

	if (!module)
	{
		g_warning("Could not load module %s\n%s", elt->module,
				  g_module_error());
		return NULL;
	}
	else
	{
		if (!g_module_symbol
			(module, "gm_module_start", (gpointer *) & (elt->gm_module_start)))
		{
			elt->gm_module_start = NULL;
			g_warning("Could not get function gm_module_start from %s\n%s",
					  elt->module, g_module_error());
			return NULL;
		}

		if (!g_module_symbol
			(module, "gm_module_stop", (gpointer *) & (elt->gm_module_stop)))
		{
			elt->gm_module_stop = NULL;
			g_warning("Could not get function gm_module_stop from %s\n%s",
					  elt->module, g_module_error());
			return NULL;
		}

		if (!g_module_symbol
			(module, "gm_module_init", (gpointer *) & (elt->gm_module_init)))
		{
			elt->gm_module_init = NULL;
			g_warning("Could not get function gm_module_init from %s\n%s",
					  elt->module, g_module_error());
			return NULL;
		}
		if (!g_module_symbol
			(module, "gm_module_get_widget",
			 (gpointer *) & (elt->gm_module_get_widget)))
		{
			elt->gm_module_get_widget = NULL;
			g_warning
				("Could not get function gm_module_get_widget from %s\n%s",
				 elt->module, g_module_error());
			return NULL;
		}
		if (elt->module_conffile != NULL)
		{
			if (!g_module_symbol
				(module, "gm_module_set_conffile",
				 (gpointer *) & (elt->gm_module_set_conffile)))
			{
				elt->gm_module_set_conffile = NULL;
				g_warning
					("Could not get function gm_module_set_conffile from %s\n%s",
					 elt->module, g_module_error());
			}
			else
			{
				elt->
					gm_module_set_conffile((const gchar *)
										   elt->module_conffile);
			}
		}
		if (!g_module_symbol
			(module, "gm_module_set_icon_size",
			 (gpointer *) & (elt->gm_module_set_icon_size)))
		{
			elt->gm_module_set_icon_size = NULL;
			g_warning
				("Could not get function gm_module_set_icon_size from %s\n%s",
				 elt->module, g_module_error());
		}
		else
		{
			elt->gm_module_set_icon_size(width, height);
		}

		if (elt->gm_module_init() != GM_SUCCES)
		{
			g_warning("Failed to initialize module %s", elt->module);
			return NULL;
		}
	}

	return elt->gm_module_get_widget();
}

static GtkWidget *gm_create_buttonbox(menu_elements *elts, int elts_index, 
								int box_width, int box_height,
								void (*processevent) (GtkWidget *, GdkEvent *,
													 menu_elements *))
{
	menu_elements *next, *cur;
	GtkWidget *button, *hbox, *vbox;
	int elts_per_col, elts_per_row, count, button_height, button_width;

	vbox = gtk_vbox_new(FALSE, 0);

	elts_per_row =
		calculateAmountOfElementsPerRow(box_width, box_height,
										   *elts->max_elts_in_single_box);
	if (elts_per_row < 1)
	{
		elts_per_row = 1;
	}

	elts_per_col = (*elts->max_elts_in_single_box) / elts_per_row;

	if (elts_per_col < 1)
	{
		elts_per_col = 1;
	}

	g_debug("no_elts: %d col: %d row: %d box: %dx%d", *elts->max_elts_in_single_box, elts_per_col, elts_per_row, box_width, box_height);

	button_height = box_height / elts_per_col;
	button_width = box_width / elts_per_row;

	// The size metric is 1024th of a point.
	fontsize = (1024 * button_width) / MAXCHARSINLABEL;

	cur = elts;
	count = 0;
	while ((cur != NULL) && (count < *elts->max_elts_in_single_box))
	{
		if ((count % elts_per_row) == 0)
		{
			hbox = gtk_hbox_new(FALSE, 0);

			gtk_container_add(GTK_CONTAINER(vbox), hbox);
		}

		next = cur->next;
		button = gm_create_button(cur, button_width, button_height, processevent);
		gtk_box_pack_start(GTK_BOX(hbox), button, TRUE, TRUE, 0);
		cur->widget = button;
		cur = next;
		count++;
	}

	return vbox;
}

GtkWidget *gm_create_buttonboxes(menu_elements *elts,
							   void (*processevent) (GtkWidget *, GdkEvent *,
													 menu_elements *))
{
	GtkWidget *boxes[10];
	GtkWidget *hbox;
	int i;
	int index = 0;
	int box_width, box_height;

	box_width = gm_calculate_box_length(screen_width, elts->menu_width);
	box_height = gm_calculate_box_length(screen_height, elts->menu_height);

	hbox = gtk_hbox_new(FALSE, 0);

	for(i=0;i<*elts->amount_of_elements;i+=*elts->max_elts_in_single_box)
	{
		g_debug("Creating box %d", index);
		boxes[index++] = gm_create_buttonbox(elts, i, box_width, box_height, processevent);
		if(index > 9)
		{
			g_debug("Maximum number of boxes reached");
			break;
		}
	}
	gtk_container_add(GTK_CONTAINER(hbox), boxes[0]);
	gtk_widget_show_all(boxes[0]);
	return hbox;
}

GtkWidget *gm_create_panel(menu_elements * elts)
{
	GtkWidget *button, *hbox, *vbox;
	int elts_per_row, count, button_width;
	int box_width, box_height;
	menu_elements *prev_elt;

	box_width = gm_calculate_box_length(screen_width, elts->menu_width);
	box_height = gm_calculate_box_length(screen_height, elts->menu_height);

	vbox = gtk_vbox_new(FALSE, 0);

	elts_per_row =
		calculateAmountOfElementsPerRow(box_width, box_height,
										   *elts->amount_of_elements);
	if (elts_per_row < 1)
	{
		elts_per_row = 1;
	}

	button_width = box_width / elts_per_row;

	count = 0;
	prev_elt = NULL;
	while (elts != NULL)
	{
		if ((count % elts_per_row) == 0)
		{
			hbox = gtk_hbox_new(FALSE, 0);

			gtk_container_add(GTK_CONTAINER(vbox), hbox);
		}
		button = createpanelelement(elts, button_width, box_height);
		if (button != NULL)
		{
			gtk_box_pack_start(GTK_BOX(hbox), button, TRUE, TRUE, 1);
			prev_elt = elts;
			elts = elts->next;
			count++;
		}
		else
		{
			// remove element from list
			if (prev_elt != NULL)
			{
				prev_elt->next = elts->next;
				elts->next = NULL;
				gm_free_menu_elements(elts);
				elts = prev_elt->next;
			}
			else
			{
				prev_elt = elts->next;
				elts->next = NULL;
				gm_free_menu_elements(elts);
				elts = prev_elt;
			}
		}
	}

	if (count == 0)
		return NULL;
	else
		return vbox;
}
