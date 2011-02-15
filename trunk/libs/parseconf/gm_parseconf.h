/**
 * \file gm_parseconf.h
 * \brief XML configuration file parser for menu elements
 *
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 */
#ifndef __GAPPMAN_PARSECONF_H__
#define __GAPPMAN_PARSECONF_H__

#include <gtk/gtk.h>
#include <libxml/xmlreader.h>

typedef int (*GM_MODULE_INIT) (void);	///< Function to initialize the
										// module
typedef GtkWidget *(*GM_MODULE_WIDGET) (void);	///< Function returning the
												// widget the module use. If
												// any/
typedef void (*GM_MODULE_SET_ICON_SIZE) (int width, int height);	///<
																	// Function 
																	// to set
																	// the
																	// width
																	// and
																	// height
																	// of the
																	// image
																	// used by 
																	// the
																	// module, 
																	// if any.
typedef void (*GM_MODULE_SET_CONFFILE) (const char *filename);	///< Function 
																// for gappman 
																// to provide
																// the
																// location of 
																// its
																// configuration 
																// file to the 
																// module.
typedef void *(*GM_MODULE_START) (void);	///< Function to start the
											// module. Should be called after
											// GM_MODULE_INIT.
typedef int (*GM_MODULE_STOP) (void);	///< Function to stop the module.
										// This should release all memory
										// occupied by the module. Gappman
										// does no garbage collection.

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
* \struct menu_element
* \brief structure to hold the attributes to create the button to start a program
*/
struct menu_element
{
	int *amount_of_elements;	///< total number of elements
	struct length *menu_width;	///< holds the width of the menu this element 
								// is a part of. Note that all elements in the 
								// same menu should point to the same length
								// struct for menu_width.
	struct length *menu_height;	///< holds the height of the menu this
								// elements is a part of. Note that all
								// elements in the same menu should point to
								// the same length struct for menu_width.
	int app_width;				///< screen resolution width that should be
								// used when the application of this
								// menu_element is started.
	int app_height;				///< screen resolution height that should be
								// used when the application of this
								// menu_element is started.
	float *hor_alignment;		///< horizontal alignment of group (NOT A
								// SINGLE WIDGET). 0.0 = left, 0.5 = center,
								// 1.0 = right
	int *vert_alignment;		///< vertical alignment of group (NOT A
								// SINGLE WIDGET). 0 = top, 1 = center, 2 =
								// bottom
	GtkWidget *widget;			///< widget associated with this
								// menu_element. Usually a GtkButton.
	const xmlChar *name;		///< holds the name of the program
	const xmlChar *exec;		///< absolute path to executable
	const xmlChar *logo;		///< absolute path to image file
	const xmlChar *module;		///< absolute path to module for panel
	const xmlChar *module_conffile;	///< absolute path to module
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
	struct menu_element *next;	///< pointer to the next menu_element
								// structure
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
* \struct length
* \brief structure to hold the length type (pixel, percentage) and its value.
*/
struct length
{
	enum length_types type;		///< Type of the value, i.e. percentage or
								// pixels?
	int value;					///< Actual length value without metric
								// indicator, e.g. % or px.
};

/**
* \typedef menu_elements
* Struct menu_element has a pointer next that can be used to create a linked list of
* menu_element structs. This typedef is merely used to make clear in the code that
* a list is used instead of a single element. 
*/
typedef struct menu_element menu_elements;

/**
* \brief load the configuration file and parses it to create the menu_elements structures.
* \param  *filename the name of the configuration file with the path
* \return int 0 if configuration file was succesfully loaded, >0 otherwise
*/
int gm_load_conf(const char *filename);

/**
* \brief relinguishes the memory occupied by menu_element structures
* \param *elt first menu_element structure
*/
void gm_free_menu_elements(menu_elements * elt);

/**
* \brief returns the total number of menu_elements
* \return integer
*/
int gm_get_number_of_elements();

/**
* \brief Get the alignment of the program menu area
* \return String: [[top|left|bottom|right|center],...]
*/
xmlChar *gm_get_alignment();

/**
* \brief Get the path of the cache location on disk
* \return string
*/
char *gm_get_cache_location();

/**
* \brief Get the name of the program as specified in the configuration file
* \return string
*/
char *gm_get_programname();

/**
* \brief returns the menu_elements structure that contains the programs
* \return pointer to menu_elements structure
*/
menu_elements *gm_get_programs();

/**
* \brief returns the menu_elements structure that contains the actions
* \return pointer to menu_elements structure
*/
menu_elements *gm_get_actions();

/**
* \brief returns the menu_elements structure that contains the panel elements
* \return pointer to menu_elements structure
*/
menu_elements *gm_get_panel();

/**
* \brief returns the menu element with the given name
* \param name the programname of the menu element
* \param programs list of menu_element structs holding the programs managed by gappman
* \return pointer to the menu element structure
*/
menu_elements *gm_search_elt_by_name(gchar * name, menu_elements * programs);

#endif
