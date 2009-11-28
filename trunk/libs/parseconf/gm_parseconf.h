/**
 * \file parseconf.c
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

typedef void (* GM_MODULE_INIT) (void);

/**
* Enumeration for length types.
* Currently supported types are: PERCENTAGE and PIXELS
*/
enum length_types {
  PERCENTAGE,
  PIXELS
};

/**
* \struct menu_element
* \brief structure to hold the attributes to create the button to start a program
*/
struct menu_element
{
  struct length *menu_width;
  struct length *menu_height;
  int app_width;
  int app_height;
  char **orientation;
	GtkWidget *widget; //!< widget associated with this menu_element. Usually a GtkButton.
  const xmlChar *name; //!< holds the name of the program
  const xmlChar *exec; //!< absolute path to executable
  const xmlChar *logo; //!< absolute path to image file
  const xmlChar *module; //!< absolute path to module for panel
  int autostart; //!< a value of 1 will start program at startup, 0 will not.
  int printlabel; //!< If set to 1 the name should be printed alongside the button. Otherwise do not print a textlabel
  char **args; //!< array of strings containing arguments that need to be passed to the executable
  int numArguments; //!< will hold the total amount of elements in the args array
	int pid; //!< should hold the process ID of the process that was started by this menu_element
  struct menu_element *next; //!< pointer to the next menu_element structure
	GM_MODULE_INIT gm_module_init; //!< pointer to the init function for a panel module
	gpointer *gm_module_start; //!< pointer to the start function for a panel module
	gpointer *gm_module_stop; //!< pointer to the stop function for a panel module
	gpointer *gm_module_set_icon_size; //!< pointer to the set icon size function for a panel module
	gpointer *gm_module_get_widget; //!< pointer to the get widget function for a panel module
};

/**
* \struct length
* \brief structure to hold the length type (pixel, percentage) and its value. 
*/
struct length
{
  enum length_types type; //!< Type of the value, i.e. percentage or pixels?
  int value; //!< Actual length value without metric indicator, e.g. % or px.
};

/**
* \typedef menu_elements
*/
typedef struct menu_element menu_elements;

/**
* \brief load the configuration file and parses it to create the menu_elements structures.
* \param  *filename the name of the configuration file with the path
* \return int 0 if configuration file was succesfully loaded, >0 otherwise 
*/
int loadConf(const char *filename);

/**
* \brief relinguishes the memory occupied by menu_element structures
* \param *elt first menu_element structure
*/
void freeMenuElements( menu_elements *elt );

/**
* \brief prints the elements of a menu_element structure
* \param *elt menu_element structure that should be printed
*/
void printMenuElements( menu_elements *elt );

/**
* \brief returns the total number of menu_elements
* \return integer
*/
int getNumberOfElements();

/**
* \brief Get the path of the cache location on disk
* \return string
*/
char* getCachelocation();

/**
* \brief Get the name of the program as specified in the configuration file
* \return string
*/
char* getProgramname();

/**
* \brief Get the alignment of the program menu area
* \return String: [[top|left|bottom|right|center],...]
*/
xmlChar* getAlignment();

/**
* \brief returns the menu_elements structure that contains the programs
* \return pointer to menu_elements structure
*/
menu_elements* getPrograms();

/**
* \brief returns the menu_elements structure that contains the actions
* \return pointer to menu_elements structure
*/
menu_elements* getActions();

/**
* \brief returns the menu_elements structure that contains the panel elements
* \return pointer to menu_elements structure
*/
menu_elements* getPanelelts();


#endif
