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
struct appwidgetinfo
{
    int PID; ///< Process ID of running app (child replaced through execvp)
    int status; ///< Process status which can be either running, sleeping, waiting, stopped, or zombie
    struct menu_element *menu_elt; ///< Pointer to menu_element structure of started application
    GtkWidget *widget; ///< Button that started the process
    struct appwidgetinfo* prev; ///< Pointer to previous appwidgetinfo
    struct appwidgetinfo* next; ///< Pointer to previous appwidgetinfo
};


/**
* \brief Returns the started applications.
* \return pointer to the appwidgetinfo struct
*/
struct appwidgetinfo* get_started_apps();

/**
* \brief updates the resolution for gappman or any other program
* \param programname string holding the name of the program to update. If NULL the default resolution for gappman is updated
* \param width new width that should be set for the program
* \param height new height that should be set for the program
*/
void update_resolution(gchar* programname, int width, int height);
