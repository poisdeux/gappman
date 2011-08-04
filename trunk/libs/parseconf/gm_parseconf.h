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

#include <gm_generic.h>

/**
* \brief load the configuration file and parses it to create the menu_elements structures.
* \param  *filename the name of the configuration file with the path
* \return int 0 if configuration file was succesfully loaded, >0 otherwise
*/
int gm_load_conf(const char *filename);

/**
* \brief relinguishes the memory occupied by a menu structure
* \param dish menu structure
*/
void gm_free_menu(gm_menu *dish);

/**
* \brief Get the alignment of the program menu area
* \return String: [[top|left|bottom|right|center],...]
*/
char *gm_get_alignment();

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
gm_menu *gm_get_programs();

/**
* \brief returns the menu_elements structure that contains the actions
* \return pointer to menu_elements structure
*/
gm_menu *gm_get_actions();

/**
* \brief returns the menu_elements structure that contains the panel elements
* \return pointer to menu_elements structure
*/
gm_menu *gm_get_panel();

/**
* \brief returns the menu element with the given name
* \param name the programname of the menu element
* \param programs list of menu_element structs holding the programs managed by gappman
* \return pointer to the menu element structure
*/
gm_menu_element *gm_search_elt_by_name(gchar *name, gm_menu *programs);

#endif
