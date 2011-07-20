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
* \return integer value
*                       0: OK
*                       1: Could not resolve hostname
*                       2: Could not connect
*                       3: Could not send message
*                       4: Could not shutdown channel/disconnect
*/
int gm_dbus_get_started_procs_from_gappman(struct proceslist **startedprocs);

/**
* \brief Connects to gappman and requests the fontsize
* \param path pointer to a string holding the configuration path
* \return integer value (GM_*) as defined in libs/generic/gm_generic.h
*/
int gm_dbus_get_confpath_from_gappman(gchar ** path);


/**
* \brief Connects to gappman and requests the fontsize
* \param fontsize adres of the pointer to an integer value (call by reference)
* \return integer value (GM_*) as defined in libs/generic/gm_generic.h
*/
int gm_dbus_get_fontsize_from_gappman(int *fontsize);

/**
* \brief Requests for the windowgeometry object
* \param width a value by reference which will hold the retrieved width from gappman
* \param height a value by reference which will hold the retrieved height from gappman
*/
int gm_dbus_get_window_geometry_from_gappman(int *width, int *height);

#endif // __GM_CONNECT_DBUS_H__
