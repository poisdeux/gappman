/**
 * \file nm_parseconf.h
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
* \brief structure to hold the attributes associated with a network action that netman should execute to check the network status
*/
struct nm_element
{
	gboolean running;			// !< Should be TRUE if exec has been executed 
								// and is still running. FALSE otherwise.
	int *amount_of_elements;	// !< total number of elements
	guint g_source_tag;
	const xmlChar *name;		// !< holds the name of the program
	const xmlChar *exec;		// !< absolute path to executable
	const xmlChar *logosuccess;	// !< absolute path to image file
	const xmlChar *logofail;	// !< absolute path to image file
	char **args;				// !< array of strings containing arguments
								// that need to be passed to the executable
	char **argv;				// !< Same as args but suited for use with
								// execv
	int numArguments;			// !< will hold the total amount of elements
								// in the args array
	int pid;					// !< should hold the process ID of the
								// process that was started by this nm_element
	int status;					// !< should hold the last exit value of the
								// executable
	int prev_status;			// !< should hold the last exit value of the
								// executable of the previous test
	int success;				// !< should hold the exit value of the
								// executable (*exec) that represents the
								// success state
	struct nm_element *next;	// !< pointer to the next nm_element structure
	GtkImage *image_success;	///< image that should be displayed when
								// check succeeds 
	GtkImage *image_fail;		///< image that should be displayed when
								// check fails
};

/**
* \var nm_elements
* Struct nm_element contains a pointer to the next nm_element. Creating a linked list.
* This typedef is merely to use the nm_elements in functions to make clear you are
* using a list of structs instead of just one.
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
void nm_free_elements(nm_elements * elt);

/**
* \brief Get the filename for the unavail image
* \return string or NULL if filename was not set
*/
const char *nm_get_filename_logounavail();

/**
* \brief Get the path of the cache location on disk
* \return string
*/
const char *nm_get_cache_location();

/**
* \brief returns the nm_elements structure that contains the stati
* \return pointer to nm_elements structure
*/
nm_elements *nm_get_stati();

/**
* \brief returns the nm_elements structure that contains the actions
* \return pointer to nm_elements structure
*/
nm_elements *nm_get_actions();
#endif
