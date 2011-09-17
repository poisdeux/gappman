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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/**
* \brief menu for layout functions
*/
typedef struct _menu gm_menu;

/**
* \brief single element from a menu
*/
typedef struct _menu_element gm_menu_element;

/**
* \brief single element from a menu
*/
typedef struct _menu_page gm_menu_page;

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
* enumeration for error/return codes
*/
typedef enum returncodes {
GM_SUCCESS, ///< no errors detected
GM_FAIL, ///< used to represent a general error
GM_NO_RANDR_EXTENSION,	///< Xorg has no support for the XRANDR extension. This is fatal for gm_changeresolution
GM_NO_SCREEN_CONFIGURATION,	///< No screen configuration could be retrieved. This is fatal for gm_changeresolution
GM_SIZE_NOT_AVAILABLE,	///< No screen sizes found that match a requested resolution. This should never happen and is only possible if wrong values are passed.
GM_COULD_NOT_RESOLVE_HOSTNAME,	///< Returned by gm_connect lib when hostname could not be resolved to an ip-address
GM_COULD_NOT_CONNECT,	///< Returned by gm_connect lib when connection with gappman failed
GM_COULD_NOT_SEND_MESSAGE,	///< Returned by gm_connect lib when sending message failed
GM_COULD_NOT_DISCONNECT,	///< Returned by gm_connect lib could not disconnect connection with gappman
GM_COULD_NOT_LOAD_FILE,	///< Could not open or read a file
GM_NET_COMM_NOT_SUPPORTED,	///< No support for network-communications
GM_COULD_NOT_RECEIVE_MESSAGE ///< returned by gm_connect lib when receiving message failed
} GmReturnCode;

/**
* Enumeration for length types.
* Currently supported types are: PERCENTAGE and PIXELS
*/
typedef enum length_types
{
	PERCENTAGE,
	PIXELS
} GmLengthType;

/**
* \struct length
* \brief structure to hold the length type (pixel, percentage) and its value.
*/
struct length
{
	GmLengthType type;		///< Type of the value, i.e. percentage or pixels?
	gint value;					///< Actual length value without metric indicator, e.g. % or px.
	gint pixels; ///< Calculated length in pixels based on type and value
};


/**
* \brief structure to hold the attributes to create the button to start a program
*/
struct _menu_element
{
	gint app_width;				///< screen resolution width that should be
								// used when the application of this
								// menu_element is started.
	gint app_height;				///< screen resolution height that should be
								// used when the application of this
								// menu_element is started.
	GtkWidget *widget;			///< widget associated with this
								// menu_element. Usually a GtkButton.
	gchar *name;		///< holds the name of the program
	gchar *exec;		///< absolute path to executable
	gchar *logo;		///< absolute path to image file
	gchar *module;		///< absolute path to module for panel
	gchar *module_conffile;	///< absolute path to module
									// configuration file
	gint autostart;				///< a value of 1 will start program at
								// startup, 0 will not.
	gint printlabel;				///< If set to 1 the name should be printed
	gchar **args;				///< arguments that need to be passed to the executable
	gint amount_of_args;			///< total amount of elements in the args array
	gint pid;					///< process ID of the process that was started by this menu_element
	gpointer data; ///< used to hold arbitrary data for callback functions
	GM_MODULE_INIT gm_module_init;	///< init function for a panel module
	GM_MODULE_START gm_module_start;	///< start function for a panel module
	GM_MODULE_STOP gm_module_stop;	///< stop function for a panel module
	GM_MODULE_SET_ICON_SIZE gm_module_set_icon_size;	///< sets icon size for a panel module
	GM_MODULE_SET_CONFFILE gm_module_set_conffile;	///< sets configuration file function for a panel module
	GM_MODULE_WIDGET gm_module_get_widget;	///< used by gappman to get the widget from a panel module
};

/**
* \brief forms a linked list to all boxes in a specific menu
*/
struct _menu_page
{
	GtkWidget *box; ///< pointer to a buttonbox
	gm_menu_page *next; ///< pointer to next menu_page in the linked list;
	gm_menu_page *prev; ///< pointer to previous menu_page in the linked list;
};

/**
* \brief structure which holds a list of menu elements and all meta-information regarding the menu
*/
struct _menu
{
	int amount_of_elements;	///< total number of elements
	int max_elts_in_single_box; ///< maximum number of elements allowed in one box.
	gm_menu_page *pages; ///< list of menu boxes
	struct length menu_width;	///< holds the width of the menu this element 
								// is a part of. Note that all elements in the 
								// same menu should point to the same length
								// struct for menu_width.
	struct length menu_height;	///< holds the height of the menu this
								// elements is a part of. Note that all
								// elements in the same menu should point to
								// the same length struct for menu_width.
	gint widget_width; ///< width of an individual widget in the menu
	gint widget_height; ///< height of an individual widget in the menu
	gint box_width;	///< width of one box (page) for this menu
	gint box_height;	///< height of one box (page) for this menu
	gint elts_per_row; ///< maximum number of elements in a single row
	float hor_alignment;		///< horizontal alignment of menu 
								// 0.0 = left, 0.5 = center, 1.0 = right
	int vert_alignment;		///< vertical alignment of menu
								// 0 = top, 1 = center, 2 = bottom
	gm_menu_element **elts; ///< list of menu elements that are part of this menu
};

/**
* \brief relinguishes the memory occupied by a menu
* \param menu menu structure
*/
void gm_menu_free(gm_menu *menu);

/**
* \brief relinguishes the memory occupied by a menu_element
* \param menu menu structure
*/
void gm_menu_element_free(gm_menu_element *elt);

/**
* \brief returns the menu element with the given name
* \param name the programname of the menu element
* \param programs list of menu_element structs holding the programs managed by gappman
* \return pointer to the menu element structure
*/
gm_menu_element *gm_menu_search_elt_by_name(gchar *name, gm_menu *programs);

/**
* \brief returns the amount of menu elements in the menu
* \param menu pointer to a gm_menu type
* \return int amount of elements in menu
*/
gint gm_menu_get_amount_of_elements(gm_menu *menu);

/**
* \brief returns name for menu element
* \param elt gm_menu_element
* \return gchar pointer
*/
gchar *gm_menu_element_get_name(gm_menu_element *elt);

/**
* \brief creates a gm_menu with default initialization
* \return gm_menu reference which should be freed with gm_menu_free
*/
gm_menu *gm_menu_create();

/**
* \brief adds a gm_menu_element to a gm_menu
* \param elt pointer to gm_menu_element that must be added to menu
* \param menu pointer to gm_menu that should be enlarged with elt
* \return TRUE on success, FALSE on failure
*/
gboolean gm_menu_add_menu_element(gm_menu_element *elt, gm_menu *menu);

/**
* \brief adds a gm_menu_page to a gm_menu
* \param elt pointer to gm_menu_page that must be added
* \param menu pointer to gm_menu that should be enlarged with elt
* \return GM_SUCCESS on success, GM_FALSE on failure
*/
GmReturnCode gm_menu_add_page(gm_menu_page *page, gm_menu *menu);

/**
* \brief sets the preferred width for the menu
* \param length_type this can be PERCENTAGE or PIXELS
* \param width positive integer number. Note that percentages are counted from 0 to 100.
* \param menu pointer to the gm_menu that should be changed
*/
void gm_menu_set_width(GmLengthType length_type, gint width, gm_menu *menu);

/**
* \brief sets the preferred height for the menu
* \param length_type this can be PERCENTAGE or PIXELS
* \param width positive integer number. Note that percentages are counted from 0 to 100.
* \param menu pointer to the gm_menu that should be changed
*/
void gm_menu_set_height(GmLengthType length_type, gint height, gm_menu *menu);

/**
* \brief sets the maximum amount of elements allowed in a single menu page
* \param amount positive integer number
* \param menu the gm_menu that should be changed
*/
void gm_menu_set_max_elts_in_single_box(gint amount, gm_menu *menu);

/**
* \brief creates a gm_menu with default initialization
* \return gm_menu reference which should be freed with gm_menu_free
*/
gm_menu_element *gm_menu_element_create(); 

/**
* \brief add an argument to the argument list for a gm_menu_element
* \param *elt gm_menu_element which should have its argument list expanded
* \param *arg argument that should be added
* \return TRUE on success, FALSE on failure
*/
gboolean gm_menu_element_add_argument(gchar *arg, gm_menu_element *elt);


/**
* \brief returns the number of arguments for the menu element
* \param *elt gm_menu_element
* \return positive integer
*/
gint gm_menu_elements_get_amount_of_arguments(gm_menu_element *elt);

/**
* \brief sets the name of the menu_element which will be displayed if printlabel is TRUE
* \param name string holding the name. Note that gm_menu_element_set_name will not allocate memory for the string. You will need to do this yourself. Memory is freed when calling gm_menu_element_free.
* \param *elt gm_menu_element which should be changed
*/
void gm_menu_element_set_name(gchar *name, gm_menu_element *elt);

/**
* \brief specifies if the name should be displayed or not
* \param bool boolean value indicating if the name should be displayed
* \param *elt gm_menu_element which should be changed
*/
void gm_menu_element_printlabel(gboolean bool, gm_menu_element *elt);

/**
* \brief set the pid in a gm_menu_element
* \param pid Process ID of the program in gm_menu_element
* \param elt gm_menu_element that should be updated
*/
void gm_menu_element_set_pid(gint pid, gm_menu_element *elt);

/**
* \brief sets the data pointer in a gm_menu_element. Most useful to pass arbitrary data to 
* a callback function
* \param data pointer to arbitrary data
* \param elt gm_menu_element that should be updated
*/
void gm_menu_element_set_data(gpointer data, gm_menu_element *elt);

/**
* \brief sets the widget pointer in a menu_element
* \param widget pointer to the widget that should be included in menu_element
* \param elt pointer to the gm_menu_element whose widget should be changed
*/
void gm_menu_element_set_widget(GtkWidget *widget, gm_menu_element *elt);

/**
* \brief Creates a new gm_menu_page and adds box as the content of the page
* \param box pointer to a GtkWidget that belongs to this page. Note that only one GtkWidget can be added to a single page. Because of this the widget will usually be a container
* \return gm_menu_page pointer
*/
gm_menu_page *gm_menu_page_create(GtkWidget *box);

/**
* \brief relinguishes the memory occupied by a menu_page
* \param page pointer to gm_menu_page
*/
void gm_menu_page_free(gm_menu_page *page);

/**
* \brief returns the next gm_menu_page from the given gm_menu_page page
* \param page pointer to gm_menu_page
* \return gm_menu_page pointer
*/
gm_menu_page *gm_menu_page_next(gm_menu_page* page);

#endif

