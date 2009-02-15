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
* \struct button
* \brief structure to hold the attributes to create a button
*/
struct button
{
  struct length *box_width;
  struct length *box_height;
  const xmlChar *labeltext; //!< holds the name of the program
  const xmlChar *executable; //!< absolute path to executable
  const xmlChar *imagefilename; //!< absolute path to image file
  char **args; //!< array of strings containing arguments that need to be passed to the executable
  int numArguments; //!< will hold the total amount of elements in the args array
  struct button *next; //!< pointer to the next button structure
};

/**
* \typedef buttons
*/
typedef struct button buttons;

/**
* \struct length
* \brief structure to hold the length value and type
*/
struct length
{
  enum length_types type; //!< Type of the value, i.e. percentage or pixels?
  int value; //!< Actual length value without metric indicator, e.g. % or px.
};

/**
* \brief load the configuration file and parses it to create the button structures.
* \param  *filename the name of the configuration file with the path
*/
void loadConf(const char *filename);

/**
* \brief relinguishes the memory occupied by button structures
* \param *elt first button structure
*/
void freeMenuElements( buttons *elt );

/**
* \brief prints the elements of a button structure
* \param *elt button structure that should be printed
*/
void printMenuElements( buttons *elt );

/**
* \brief returns the total number of buttons
*/
int getNumberOfElements();

/**
* \brief Get the width of the widget area as specified in the configuration file
* \return struct length
*/
struct length* getWidth();

/**
* \brief Get the height of the widget area as specified in the configuration file
* \return struct length
*/
struct length* getHeight();

/**
* \brief Get the alignment of the widget area
* \return String: [[top|left|bottom|right|center],...]
*/
xmlChar* getAlignment();

