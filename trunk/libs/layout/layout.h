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
* \brief Create the button layout using the available screen height and width
* \param *elts pointer to first menu_elements structure
* \param screen_height total screen height
* \param screen_width total screen width
* \param *processevent function pointer to function which should be used as callback when a button is pressed.
*/
GtkWidget* createbuttons( menu_elements *elts, int screen_width, int screen_height, gboolean(*processevent)(GtkWidget*, GdkEvent*, menu_elements*));

