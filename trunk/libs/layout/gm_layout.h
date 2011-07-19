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
* Where *data will be the data you passed on to gm_create_empty_button in *data.
*
*/

#ifndef __GAPPMAN_LAYOUT_H__
#define __GAPPMAN_LAYOUT_H__

#include <gtk/gtk.h>
#include <gm_generic.h>

/**
* \brief destroys widget only if mousebutton or spacebar is pressed
* \param dummy widget that emitted event
* \param event event signal that triggered the function
* \param widget widget that should be destroyed
*/
void gm_destroy_widget(GtkWidget * dummy, GdkEvent * event,
					   GtkWidget * widget);

/**
* \brief checks if spacebar or mousebutton was pressed to generate the event
* \param event pointer to the GdkEvent that was generated
* \return TRUE if spacebar or mousebutton generated the event. FALSE otherwise
*/
gboolean check_key(GdkEvent * event);

/**
* \brief quits program only if mousebutton or spacebar is pressed
* \param dummy widget that emitted event
* \param event event signal that triggered the function
*/
void gm_quit_program(GtkWidget * dummy, GdkEvent * event);

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
GtkWidget *gm_load_image(const char *elt_name, const char *elt_logo,
						 const char *cacheloc, const char *programname,
						 int max_width, int max_height);

/**
* \brief shows a question dialog with regards to gappman fontsize.
* \param message pointer to char that will hold the general question
* \param msg_button1 pointer to char that will hold the message for the first button.
* \param callback1 callback function that should be called when the first button is pressed. Use NULL to disable using a callbackfunction for the first button
* \param msg_button2 pointer to char that will hold the message for the second button.
* \param data1 void pointer pointing to any data that should be passed as an argument when calling the callbackfunction callback1
* \param callback2 callback function that should be called when the second button is pressed. Use NULL to disable using a callbackfunction for the second button
* \param data2 void pointer pointing to any data that should be passed as an argument when calling the callbackfunction callback2
* \param mainwin pointer to parent window GtkWidget that called gm_show_confirmation_dialog. If NULL dialog is positioned in the center.
*/
void gm_show_confirmation_dialog(const gchar * message,
								 const gchar * msg_button1, void *callback1,
								 void *data1, const gchar * msg_button2,
								 void *callback2, void *data2,
								 GtkWidget * mainwin);

/**
* \brief shows an error dialog with regards to gappman fontsize.
* \param message pointer to char that will hold the message
* \param mainwin pointer to parent window GtkWidget that called gm_show_error_dialog. If NULL dialog is positioned in the center.
* \param callback pointer to callback function that should be called when Close button is pressed. If NULL the default action will be taken which simply destroys this error dialog.
*/
void gm_show_error_dialog(const gchar * message, GtkWidget * mainwin,
						  void *callback);

/**
* \brief returns the generic fontsize for gappman
* \return int the fontsize
*/
int gm_get_fontsize();

/**
* \brief sets the generic fontsize for gappman
* \param size the fontsize
*/
void gm_set_fontsize(int size);

/**
* \brief sets the screen size that should be used by gappman
* \param width width of the screen
* \param height height of the screen
*/
void gm_set_window_geometry(int width, int height);

/**
* \brief creates a basic button
* \param buttontext string that should be used as label
* \param callbackfunc void pointer to callback function that should be called when button is clicked
* \param data void pointer to arguments that should be passed to callback function 
* \return GtkWidget pointer to the new button
*/
GtkWidget *gm_create_label_button(gchar * buttontext, void *callbackfunc,
								  void *data);

/**
* \brief Create a single button possibly with a label
* \param elt menu_element struct that contains the logo image filename.
* \param max_width button width
* \param max_height button height
* \param processevent callback function that will be called when button is pressed
* \return GtkWidget pointer to the new button
*/
GtkWidget *gm_create_button(struct menu_element *elt, int max_width, int max_height,
							void (*processevent) (GtkWidget *, GdkEvent *,
												  struct menu_element *));

/**
* \brief Create a single empty button
* \param callbackfunc callback function that will be called when button is pressed. 
* \param data data to be parsed as argument when calling callbackfunc
* \return GtkWidget pointer to the new button
*/
GtkWidget *gm_create_empty_button(void *callbackfunc, void *data);

/**
* \brief Creates the button layout using the available screen height and width
* \param dish pointer to struct menu
* \param processevent function pointer to function which should be used as callback when a button is pressed.
* \return GtkWidget pointer to a hbox that contains one or more hboxes 
*/
GtkWidget *gm_create_menu(struct menu *dish,
                 void (*processevent) (GtkWidget *, GdkEvent *,
                           struct menu_element *));

/**
* \brief Creates the panel layout using the available screen height and width
* \param dish pointer to menu structure that holds the panel elements
* \return GtkWidget pointer to container holding the panel
*/
GtkWidget *gm_create_panel(struct menu *dish);

#endif
