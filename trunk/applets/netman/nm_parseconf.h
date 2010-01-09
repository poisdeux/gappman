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
#ifndef __GAPPMAN_NM_PARSECONF_H__
#define __GAPPMAN_NM_PARSECONF_H__

#include <gtk/gtk.h>
#include <libxml/xmlreader.h>

/**
* \struct nm_element
* \brief structure to hold the attributes to create the button to start a program
*/
struct nm_element
{
	int *amount_of_elements; //!< total number of elements
  const xmlChar *name; //!< holds the name of the program
  const xmlChar *exec; //!< absolute path to executable
  const xmlChar *logosuccess; //!< absolute path to image file
  const xmlChar *logofail; //!< absolute path to image file
  char **args; //!< array of strings containing arguments that need to be passed to the executable
  int numArguments; //!< will hold the total amount of elements in the args array
	int pid; //!< should hold the process ID of the process that was started by this nm_element
	int success; //!< should hold the exit value of the executable (*exec) that represents the success state
  struct nm_element *next; //!< pointer to the next nm_element structure
};

/**
* \typedef nm_elements
*/
typedef struct nm_element nm_elements;

/**
* \brief load the configuration file and parses it to create the nm_elements structures.
* \param  *filename the name of the configuration file with the path
* \return int 0 if configuration file was succesfully loaded, >0 otherwise 
*/
int nm_load_conf(const char *filename);

/**
* \brief relinguishes the memory occupied by nm_element structures
* \param *elt first nm_element structure
*/
void nm_free_elements( nm_elements *elt );

/**
* \brief Get the path of the cache location on disk
* \return string
*/
char* nm_get_cache_location();

/**
* \brief returns the nm_elements structure that contains the stati 
* \return pointer to nm_elements structure
*/
nm_elements* nm_get_stati();

/**
* \brief returns the nm_elements structure that contains the actions
* \return pointer to nm_elements structure
*/
nm_elements* nm_get_actions();
#endif
