/**
 * \file gm_connect-dbus.h
 * \brief generic functions to retrieve or sent information from or to gappman
 *
 *
 * GPL v2
 *
 * \author
 *   Martijn Brekhof <m.brekhof@gmail.com>
 *
*/

#ifndef __GM_CONNECT_DBUS_H__
#define __GM_CONNECT_DBUS_H__

/**
* \brief Connects to gappman and requests the proceslist
* \param startedprocs adres of the pointer to a proceslist structure (call by reference). Needs to be freed after use with gm_free_proceslist(*startedprocs)
* \return integer value (GM_*) as defined in libs/generic/gm_generic.h
*/
int gm_dbus_get_started_procs_from_gappman(struct proceslist **startedprocs);

/**
* \brief Connects to gappman and requests the fontsize
* \param path pointer to a string holding the configuration path
* \return integer value (GM_*) as defined in libs/generic/gm_generic.h
*/
int gm_dbus_get_confpath_from_gappman(gchar **path);


/**
* \brief Connects to gappman and requests the fontsize
* \param fontsize adres of the pointer to an integer value (call by reference)
* \return integer value (GM_*) as defined in libs/generic/gm_generic.h
*/
int gm_dbus_get_fontsize_from_gappman(int *fontsize);

/**
* \brief Connects to gappman
* \param sockfd pointer to int which will hold the socket filedescriptor
* \return filedescriptor
*/
int gm_dbus_connect_to_gappman(int *sockfd);

#endif //__GM_CONNECT_DBUS_H__
