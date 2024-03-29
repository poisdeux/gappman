/**
 * \file appmanager.h
 * \brief The main application manager better known as gappman
 * Note that these exported functions should not be used. All
 * generic code that should be used by other components like
 * applets should be included in one of the libs.
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 */

#include <gtk/gtk.h>
#include <gm_parseconf.h>

/**
* \brief Struct that holds all relevant info about started applications
*/
struct process_info
{
	int PID;					///< Process ID of running app (child
								// replaced through execvp)
	int status;					///< Process status which can be either
								// running, sleeping, waiting, stopped, or
								// zombie
	gm_menu_element *menu_elt;	///< Pointer to menu_element structure of 
									// started application
	struct process_info *prev;	///< Pointer to previous process_info
	struct process_info *next;	///< Pointer to next process_info
};

/**
* \brief Struct that holds all layout/window related information
*/
struct metadata
{
	gchar* conffile;
  gint keep_below;
  gint windowed;
  gint screen_width;
  gint screen_height;
  gint window_width;
  gint window_height;
};

/**
* \brief Returns the started applications.
* \return pointer to the process_info struct
*/
struct process_info *appmanager_get_started_apps();

/**
* \brief updates the resolution for gappman or any other program
* \param programname string holding the name of the program to update. If NULL the default resolution for gappman is updated
* \param width new width that should be set for the program
* \param height new height that should be set for the program
*/
void appmanager_update_resolution(gchar * programname, int width, int height);

/**
* \brief Returns the metadata from gappman
* \return pointer to the metadata struct
*/
struct metadata *appmanager_get_metadata();
