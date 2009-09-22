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

/**
* \brief callback function to quit the program
* \param *widget pointer to widget to destroy
* \param data mandatory argument for callback function, may be NULL.
*/
static void layout_destroy( GtkWidget *widget, gpointer   data );

/**
* \brief Creates a box with an image and labeltext on the right of it.
* \param imagefile filename of image on disk
* \param labeltext string containing label
* \param max_width maximum allowed width the image may have
* \param max_height maximum allowed height the image may have
* \return GtkWidget pointer
*/
GtkWidget* image_label_box_hor (menu_elements *elt, gchar* labeltext, int max_width, int max_height);

/**
* \brief Creates a box with an image and labeltext below it.
* \param imagefile filename of image on disk
* \param labeltext string containing label
* \param max_width maximum allowed width the image may have
* \param max_height maximum allowed height the image may have
* \return GtkWidget pointer
*/
GtkWidget* image_label_box_vert (menu_elements *elt, gchar* labeltext, int fontsize, int max_width, int max_height);

/**
* \brief loads image and scales it making sure the image fits inside
* max_width*max_height maintaining the correct aspect ratio
* \param imagefile filename of the image on disk
* \param max_width maximum width image may have
* \param max_height maximum height image may have
* \return GtkWidget pointer to image
*/
GtkWidget* load_image(menu_elements *elt, int max_width, int max_height);

/**
* \brief Create a single button possibly with a label
* \param *elt pointer to menu_element struct that contains the logo image filename.
* \param fontsize size of font used to set the label
* \param max_width button width
* \param max_height button height 
* \param *processevent callback function that will be called when button is pressed
*/
GtkWidget* createbutton ( menu_elements *elt, int fontsize, int max_width, int max_height, gboolean (*processevent)(GtkWidget*, GdkEvent*, menu_elements*) );

/**
* \brief Create a single empty button
* \param max_width button width
* \param max_height button height 
* \param *processevent callback function that will be called when button is pressed
*/
GtkWidget* create_empty_button ( int max_width, int max_height, gboolean (*processevent)(GtkWidget*, GdkEvent*, void* ), void *data);

/**
* \brief Create the button layout using the available screen height and width
* \param *elts pointer to first menu_elements structure
* \param screen_height total screen height
* \param screen_width total screen width
* \param *processevent function pointer to function which should be used as callback when a button is pressed.
*/
GtkWidget* createbuttons( menu_elements *elts, int screen_width, int screen_height, gboolean(*processevent)(GtkWidget*, GdkEvent*, menu_elements*));


