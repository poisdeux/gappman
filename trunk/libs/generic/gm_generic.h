/**
 * \file gm_generic.h
 * \brief generic stuff that is generally used by all gappman code
 *
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 */
#ifndef __GAPPMAN_GENERIC_H__
#define __GAPPMAN_GENERIC_H__

#include <gtk/gtk.h>

#ifndef SYSCONFDIR
#define SYSCONFDIR "/etc/gappman"	///< default location for the
									// configuration files
#endif

#define GM_SUCCES 0				///< no errors detected
#define GM_FAIL 9				///< used to represent a general error
#define GM_NO_RANDR_EXTENSION 1	///< Xorg has no support for the XRANDR
								// extension. This is fatal for
								// gm_changeresolution
#define GM_NO_SCREEN_CONFIGURATION 2	///< No screen configuration could be 
										// retrieved. This is fatal for
										// gm_changeresolution
#define GM_SIZE_NOT_AVAILABLE 3	///< No screen sizes found that match a
								// requested resolution. This should never
								// happen and is only possible if wrong values 
								// are passed.
#define GM_COULD_NOT_RESOLVE_HOSTNAME 4	///< Returned by gm_connect lib when
										// hostname could not be resolved to
										// an ip-address
#define GM_COULD_NOT_CONNECT 5	///< Returned by gm_connect lib when
								// connection with gappman failed
#define GM_COULD_NOT_SEND_MESSAGE 6	///< Returned by gm_connect lib when
									// sending message failed
#define GM_COULD_NOT_DISCONNECT 7	///< Returned by gm_connect lib could not 
									// disconnect connection with gappman
#define GM_COULD_NOT_LOAD_FILE 8	///< Could not open or read a file
#define GM_NET_COMM_NOT_SUPPORTED 9	///< No support for
									// network-communications
#define GM_COULD_NOT_RECEIVE_MESSAGE 10 ///< returned by gm_connect lib when receiving message failed

/**
* \brief Function to initialize the module
*/
typedef int (*GM_MODULE_INIT) (void);

/**
* \brief Function returning the widget the module use. If any.
*/
typedef GtkWidget *(*GM_MODULE_WIDGET) (void);

/**
* \brief Function to set the width and height of the image used 
* by the module, if any.
*/
typedef void (*GM_MODULE_SET_ICON_SIZE) (int width, int height);	

/**
* \brief Function for gappman to provide the location of its configuration
* file to the module.
*/
typedef void (*GM_MODULE_SET_CONFFILE) (const char *filename);	

/**
* \brief Function to start the module. Should be called after GM_MODULE_INIT.
*/
typedef void *(*GM_MODULE_START) (void);

/**
* \brief Function to stop the module. This should release all memory occupied by the module. Gappman does no garbage collection.
*/
typedef int (*GM_MODULE_STOP) (void);	

/**
* Enumeration for length types.
* Currently supported types are: PERCENTAGE and PIXELS
*/
enum length_types
{
	PERCENTAGE,
	PIXELS
};

/**
* \struct length
* \brief structure to hold the length type (pixel, percentage) and its value.
*/
struct length
{
	enum length_types type;		///< Type of the value, i.e. percentage or
								// pixels?
	int value;					///< Actual length value without metric
								// indicator, e.g. % or px.
	int pixels; ///< Calculated length in pixels based on type and value
};


/**
* \struct menu_element
* \brief structure to hold the attributes to create the button to start a program
*/
struct menu_element
{
	int app_width;				///< screen resolution width that should be
								// used when the application of this
								// menu_element is started.
	int app_height;				///< screen resolution height that should be
								// used when the application of this
								// menu_element is started.
	GtkWidget *widget;			///< widget associated with this
								// menu_element. Usually a GtkButton.
	const char *name;		///< holds the name of the program
	const char *exec;		///< absolute path to executable
	const char *logo;		///< absolute path to image file
	const char *module;		///< absolute path to module for panel
	const char *module_conffile;	///< absolute path to module
									// configuration file
	int autostart;				///< a value of 1 will start program at
								// startup, 0 will not.
	int printlabel;				///< If set to 1 the name should be printed
								// alongside the button. Otherwise do not
								// print a textlabel
	char **args;				///< array of strings containing arguments
								// that need to be passed to the executable
	int numArguments;			///< will hold the total amount of elements
								// in the args array
	int pid;					///< should hold the process ID of the
								// process that was started by this
								// menu_element
	GM_MODULE_INIT gm_module_init;	///< pointer to the init function for a
									// panel module
	GM_MODULE_START gm_module_start;	///< pointer to the start function
										// for a panel module
	GM_MODULE_STOP gm_module_stop;	///< pointer to the stop function for a
									// panel module
	GM_MODULE_SET_ICON_SIZE gm_module_set_icon_size;	///< pointer to the
														// set icon size
														// function for a
														// panel module
	GM_MODULE_SET_CONFFILE gm_module_set_conffile;	///< pointer to the set
													// configuration file
													// function for a panel
													// module
	GM_MODULE_WIDGET gm_module_get_widget;	///< pointer to the get widget
											// function for a panel module
};

/**
* \struct menu_box
* \brief forms a linked list to all boxes in a specific menu
*/
struct menu_box
{
	GtkWidget *box; ///< pointer to a buttonbox
	struct menu_box *next; ///< pointer to next menu_box in the linked list;
	struct menu_box *prev; ///< pointer to previous menu_box in the linked list;
};

/**
* \struct menu
* \brief structure which holds a list of menu elements and all meta-information regarding the menu
*/
struct menu
{
	int amount_of_elements;	///< total number of elements
	int max_elts_in_single_box; ///< maximum number of elements allowed in one box.
	struct menu_box *boxes; ///< list of menu boxes
	struct length menu_width;	///< holds the width of the menu this element 
								// is a part of. Note that all elements in the 
								// same menu should point to the same length
								// struct for menu_width.
	struct length menu_height;	///< holds the height of the menu this
								// elements is a part of. Note that all
								// elements in the same menu should point to
								// the same length struct for menu_width.
	float hor_alignment;		///< horizontal alignment of menu 
								// 0.0 = left, 0.5 = center, 1.0 = right
	int vert_alignment;		///< vertical alignment of menu
								// 0 = top, 1 = center, 2 = bottom
	struct menu_element *elts; ///< list of menu elements that are part of this menu
};
#endif
