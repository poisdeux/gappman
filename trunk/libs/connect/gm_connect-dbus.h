/**
 * \file gm_connect.h
 * \brief generic functions to retrieve or sent information from or to gappman
 *
 *
 * GPL v2
 *
 * \author
 *   Martijn Brekhof <m.brekhof@gmail.com>
 *
 * \todo Rewrite to support D-Bus
 * \todo add functions to retrieve proceslist
*/

#ifndef __GM_CONNECT_DBUS_H__
#define __GM_CONNECT_DBUS_H__

/**
* \brief Connects to gappman and requests the proceslist
* \param portno portnumber gappman listens to
* \param hostname servername of host that runs gappman
* \param startedprocs adres of the pointer to a proceslist structure (call by reference). Needs to be freed after use with gm_free_proceslist(*startedprocs)
* \return integer value
*                       0: OK
*                       1: Could not resolve hostname
*                       2: Could not connect
*                       3: Could not send message
*                       4: Could not shutdown channel/disconnect
*/
int gm_dbus_get_started_procs_from_gappman(int portno, const char* hostname, struct proceslist **startedprocs);

/**
* \brief Connects to gappman and requests the fontsize
* \param portno portnumber gappman listens to
* \param hostname servername of host that runs gappman
* \param path pointer to a string holding the configuration path
* \return integer value (GM_*) as defined in libs/generic/gm_generic.h
*/
int gm_dbus_get_confpath_from_gappman(int portno, const char* hostname, gchar **path);


/**
* \brief Connects to gappman and requests the fontsize
* \param portno portnumber gappman listens to
* \param hostname servername of host that runs gappman
* \param fontsize adres of the pointer to an integer value (call by reference)
* \return integer value (GM_*) as defined in libs/generic/gm_generic.h
*/
int gm_dbus_get_fontsize_from_gappman(int portno, const char* hostname, int *fontsize);

/**
* \brief Connects to gappman
* \param portno portnumber gappman listens to
* \param hostname servername of host that runs gappman
* \param sockfd pointer to int which will hold the socket filedescriptor
* \return filedescriptor
*/
int gm_dbus_connect_to_gappman(int portno, const char* hostname, int *sockfd);

#endif //__GM_CONNECT_DBUS_H__
