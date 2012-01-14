/**
 * \file applets/digitalclock/parseconf.h
 * \brief XML configuration file parser 
 *
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 */
#ifndef __GAPPMAN_DC_PARSECONF_H__
#define __GAPPMAN_DC_PARSECONF_H__

#include <gtk/gtk.h>
#include <libxml/xmlreader.h>

/**
* \struct element
* \brief structure to hold the configuration 
*/
struct element
{
	gint show_date;
};

/**
* \brief load the configuration file and parses it to create the nm_elements structures.
* \param  *filename the name of the configuration file with the path
* \return int 0 if configuration file was succesfully loaded, >0 otherwise
*/
int load_conf(const char *filename);

/**
* \brief returns if date should be shown alongside the timee
* \return int 1 if date should be shown, 0 otherwise
*/
int get_show_date();
#endif
