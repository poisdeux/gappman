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
* \brief Get the path of the cache location on disk
* \return string
*/
gchar *gm_parseconf_get_cache_location();

/**
* \brief Get the name of the program as specified in the configuration file
* \return string
*/
gchar *gm_parseconf_get_programname();

#endif
