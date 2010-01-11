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

#include <libxml/xmlreader.h>

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
  const xmlChar *name; //!< holds the name of the program
  const xmlChar *exec; //!< absolute path to executable
  const xmlChar *logo; //!< absolute path to image file
  int autostart; //!< a value of 1 will start program at startup, 0 will not.
  int printlabel; //!< If set to 1 the label should be printed. Otherwise do not print a textlabel
  char **args; //!< array of strings containing arguments that need to be passed to the executable
  int numArguments; //!< will hold the total amount of elements in the args array
  struct menu_element *next; //!< pointer to the next menu_element structure
	int status; //!< integer specifying if the program is running, sleeping, stopped, wating, or a zombie.
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
*/
void loadConf(const char *filename);

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
