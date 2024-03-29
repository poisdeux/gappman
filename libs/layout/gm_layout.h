/**
* \file gm_layout.h
*
*
* GPL v2
*
* Authors:
*   Martijn Brekhof <m.brekhof@gmail.com>
*
* Format of the callback function should be: 
* functionname( GtkWidget *widget, GdkEvent *event, void *data ).
* Where *data will be the data you passed on to gm_layout_create_empty_button in *data.
*
*/

#ifndef __GAPPMAN_LAYOUT_H__
#define __GAPPMAN_LAYOUT_H__

#include <gtk/gtk.h>
#include <gm_generic.h>

/**
* \brief checks if spacebar or mousebutton was pressed to generate the event
* \param event pointer to the GdkEvent that was generated
* \return TRUE if spacebar or mousebutton generated the event. FALSE otherwise
*/
gboolean gm_layout_check_key(GdkEvent * event);

/**
* \brief loads image and scales it making sure the image fits inside
* max_width x max_height maintaining the correct aspect ratio
* \param elt_name name of the element
* \param elt_logo filename of the logo image to load
* \param cacheloc directory where the cached images are kept
* \param programname name of the main program that calls this function (i.e. gappman, netman, etc.)
* \param max_width maximum width image may have
* \param max_height maximum height image may have
* \return GtkWidget pointer to image
*/
GtkWidget *gm_layout_load_image(gchar *elt_name, gchar *elt_logo,
						 gchar *cacheloc, gchar *programname,
						 gint max_width, gint max_height);

/**
* \brief shows a question dialog with regards to gappman fontsize.
* \param message pointer to char that will hold the general question
* \param msg_button1 pointer to char that will hold the message for the first button.
* \param callback1 callback function that should be called when the first button is pressed. Use NULL to disable using a callbackfunction for the first button
* \param msg_button2 pointer to char that will hold the message for the second button.
* \param data1 void pointer pointing to any data that should be passed as an argument when calling the callbackfunction callback1
* \param callback2 callback function that should be called when the second button is pressed. Use NULL to disable using a callbackfunction for the second button
* \param data2 void pointer pointing to any data that should be passed as an argument when calling the callbackfunction callback2
* \param mainwin pointer to parent window GtkWidget that called gm_layout_show_question_dialog. If NULL dialog is positioned in the center.
*/
void gm_layout_show_question_dialog(gchar * message,
								 gchar * msg_button1, void *callback1,
								 void *data1, gchar * msg_button2,
								 void *callback2, void *data2,
								 GtkWidget * mainwin);

/**
* \brief shows an error dialog with regards to gappman fontsize.
* \param message pointer to char that will hold the message
* \param mainwin pointer to parent window GtkWidget that called gm_layout_show_error_dialog. If NULL dialog is positioned in the center.
* \param callback pointer to callback function that should be called when Close button is pressed. If NULL the default action will be taken which simply destroys this error dialog.
*/
void gm_layout_show_error_dialog(gchar * message, GtkWidget * mainwin,
						  void *callback);

/**
* \brief shows an error dialog corresponding to the GmReturnCode
* \param code the GmReturnCode that should be displayed
*/
void gm_layout_show_error(GmReturnCode code);

/**
* \brief returns the fontsize used by the layout manager 
* \return int the fontsize
*/
int gm_layout_get_fontsize();

/**
* \brief sets the fontsize for the layout manager
* \param size the fontsize
*/
void gm_layout_set_fontsize(gint size);

/**
* \brief gets the window width and height that is used by the layout manager to calculate the layout
* \param width reference to integer containing the window width 
* \param height reference to integer containing the window height
*/
void gm_layout_get_window_geometry(gint *width, gint *height);

/**
* \brief sets the window width and height that should be used by the layout manager to calculate the layout
* \param width width of the screen
* \param height height of the screen
*/
void gm_layout_set_window_geometry(gint width, gint height);

/**
* \brief creates a widget that will hold the specified text
* \param text the text that should be printed on the label
* \return GtkWidget
*/
GtkWidget *gm_layout_create_label(gchar *text);

/**
* \brief creates a basic button
* \param buttontext string that should be used as label
* \param callbackfunc void pointer to callback function that should be called when button is clicked
* \param data void pointer to arguments that should be passed to callback function 
* \return GtkWidget pointer to the new button
*/
GtkWidget *gm_layout_create_label_button(gchar * buttontext, void *callbackfunc,
								  void *data);

/**
* \brief Create a single button possibly with a label
* \param elt menu_element struct that contains the logo image filename.
* \param max_width button width
* \param max_height button height
* \param processevent callback function that will be called when button is pressed
* \return GtkWidget pointer to the new button
*/
GtkWidget *gm_layout_create_button(gm_menu_element *elt, int max_width, int max_height,
							void (*processevent) (GtkWidget *, GdkEvent *,
												  gm_menu_element *));

/**
* \brief Create a single empty button
* \param callbackfunc callback function that will be called when button is pressed. 
* \param data data to be parsed as argument when calling callbackfunc
* \return GtkWidget pointer to the new button
*/
GtkWidget *gm_layout_create_empty_button(void *callbackfunc, void *data);

/**
* \brief Calculates the sizes needed to correctly draw a menu. Note that if maximum elements in a single box has not been explicitly set using gm_menu_set_max_elts_in_single_box it will be set equal to the amount of elements in the menu. 
* \param menu pointer to gm_menu for which the sizes should be calculated
*/
void gm_layout_calculate_sizes(gm_menu *menu);

/**
* \brief Creates the widget layout with regards to the window height and width. Note that if maximum elements in a single box has not been explicitly set using gm_menu_set_max_elts_in_single_box it will be set equal to the amount of elements in the menu.
* \param menu pointer to struct menu
* \return GtkWidget pointer to a hbox that contains one or more hboxes 
*/
GtkWidget *gm_layout_create_menu(gm_menu *menu);

/**
* \brief Creates a container that holds all widgets. The container will be sized with respect to the window geometry
* as set by gm_layout_set_window_geometry.
* \param widgets null-terminated array of GtkWidgets.
* \return GtkWidget pointer to the container
*/
GtkWidget *gm_layout_create_box(GtkWidget **widgets);

/**
* \brief Calculates fontsize based on either window width or a message
* \param message string holding the message for which the fontsize must be calculated.
*        It uses the window width to determine the correct fontsize. If NULL only the
*        window_width will be used to calculate the fontsize.
* \return gint fontsize in points multiplied by 1024.
*/
gint gm_layout_calculate_fontsize(gchar *message);

#endif
