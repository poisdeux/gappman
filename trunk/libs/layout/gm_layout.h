/***
 * \file layout.h
 *
 *
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 */
#ifndef __GAPPMAN_LAYOUT_H__
#define __GAPPMAN_LAYOUT_H__

#include <gtk/gtk.h>
#include <gm_parseconf.h>

/**
* \brief loads image and scales it making sure the image fits inside
* max_width*max_height maintaining the correct aspect ratio
* \param elt_name name of the element
* \param elt_logo filename of the logo image to load 
* \param cacheloc directory where the cached images are kept
* \param programname name of the main program that calls this function (i.e. gappman, netman, etc.)
* \param max_width maximum width image may have
* \param max_height maximum height image may have
* \return GtkWidget pointer to image
*/
GtkWidget* gm_load_image(char* elt_name, char* elt_logo, char* cacheloc, char* programname, int max_width, int max_height);

/**
* \brief shows an error dialog with regards to gappman fontsize
* \param message pointer to char that will hold the message
* \param mainwin pointer to main window GtkWidget
* \param callback pointer to callback function that should be called when OK button is pressed
*/
void gm_show_error_dialog(const gchar* message, GtkWidget *mainwin, void* callback);

/**
* \brief returns the generic fontsize for gappman
* \return int the fontsize
*/
int gm_get_fontsize();

/**
* \brief sets the generic fontsize for gappman
* \param size the fontsize
*/
void gm_set_fontsize( int size );

/**
* \brief sets the screen size that should be used by gappman
* \param width width of the screen
* \param height height of the screen
*/
void gm_set_window_geometry(int width, int height);

/**
* \brief creates a basic cancel button
* \param void pointer to callback function that should be called when button is clicked
* \return GtkWidget pointer to the new button
*/
GtkWidget *gm_create_cancel_button(void *callbackfunc, void *data);

/**
* \brief Create a single button possibly with a label
* \param *elt pointer to menu_element struct that contains the logo image filename.
* \param fontsize size of font used to set the label
* \param max_width button width
* \param max_height button height 
* \param *processevent callback function that will be called when button is pressed
*/
GtkWidget* gm_createbutton ( menu_elements *elt, int fontsize, int max_width, int max_height, gboolean (*processevent)(GtkWidget*, GdkEvent*, menu_elements*) );

/**
* \brief Create a single empty button
* \param max_width button width
* \param max_height button height 
* \param *processevent callback function that will be called when button is pressed
*/
GtkWidget* gm_create_empty_button ( int max_width, int max_height, gboolean (*processevent)(GtkWidget*, GdkEvent*, void*), void *data);

/**
* \brief Create the button layout using the available screen height and width
* \param *elts pointer to first menu_elements structure
* \param *processevent function pointer to function which should be used as callback when a button is pressed.
*/
GtkWidget* gm_createbuttons( menu_elements *elts, gboolean(*processevent)(GtkWidget*, GdkEvent*, menu_elements*));

/**
* \brief Creates the panel layout using the available screen height and width
* \param *elts pointer to first menu_elements structure
* \return GtkWidget pointer to container holding the panel
*/
GtkWidget* gm_createpanel( menu_elements *elts);

#endif
