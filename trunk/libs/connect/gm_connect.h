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
 * \bug If appmanager is compiled with d-bus support the UI will fail to draw some widgets
*/

#ifndef __GAPPMAN_CONNECT_H__
#define __GAPPMAN_CONNECT_H__

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib.h>

/**
* \brief struct to hold the process ID and program name retrieved from Gappman
*/
struct proceslist {
    int pid;    ///< process ID of proces started by gappman
    gchar* name; ///< programname as known by gappman
    struct proceslist* prev; ///< pointer to previous proces in proceslist
};

#if defined(WITH_DBUS_SUPPORT)
#include "gm_connect-dbus.h"
#else
#include "gm_connect-socket.h"
#endif //WITH_DBUS_SUPPORT

/**
* \brief Frees the proceslist structure
* \param procslist pointer to the last element of the proceslist linked list
*/
void gm_free_proceslist(struct proceslist* procslist);

/**
* \brief Connects to gappman and requests the proceslist
* \param portno	portnumber gappman listens to. Note, this is actually not used when calling this function using the dbus version.
* \param hostname servername of host that runs gappman. Note, this is actually not used when calling this function using the dbus version.
* \param startedprocs adres of the pointer to a proceslist structure (call by reference). Needs to be freed after use with gm_free_proceslist(*startedprocs)
* \return integer value
*                       0: OK
*                       1: Could not resolve hostname
*                       2: Could not connect
*                       3: Could not send message
*                       4: Could not shutdown channel/disconnect
*/
int gm_get_started_procs_from_gappman(int portno, const char* hostname, struct proceslist **startedprocs);

/**
* \brief Connects to gappman and requests the fontsize
* \param portno	portnumber gappman listens to. Note, this is actually not used when calling this function using the dbus version.
* \param hostname servername of host that runs gappman. Note, this is actually not used when calling this function using the dbus version.
* \param path pointer to a string holding the configuration path
* \return integer value (GM_*) as defined in libs/generic/gm_generic.h
*/
int gm_get_confpath_from_gappman(int portno, const char* hostname, gchar **path);


/**
* \brief Connects to gappman and requests the fontsize
* \param portno	portnumber gappman listens to. Note, this is actually not used when calling this function using the dbus version.
* \param hostname servername of host that runs gappman. Note, this is actually not used when calling this function using the dbus version.
* \param fontsize adres of the pointer to an integer value (call by reference)
* \return integer value (GM_*) as defined in libs/generic/gm_generic.h
*/
int gm_get_fontsize_from_gappman(int portno, const char* hostname, int *fontsize);

/**
* \brief Connects to gappman using gm_socket_connect_to_gappman. You only need to use this function if your program using the socket version.
* \param portno portnumber gappman listens to
* \param hostname servername of host that runs gappman
* \param sockfd pointer to int which will hold the socket filedescriptor
* \return filedescriptor
*/
int gm_connect_to_gappman(int portno, const char* hostname, int *sockfd);

/**
* \brief calls gm_socket_send_and_receive_message to connect to gappman and send a message and receive one or more answers.
* \param portno portnumber gappman listens to
* \param hostname servername of host that runs gappman
* \param msg the message that should be sent to gappman
* \param callbackfunc callback function that should handle each message received from gappman. If NULL no messages will be received.
* \return integer value as defined in libs/generic/gm_generic.h
*/
int gm_send_and_receive_message(int portno, const char* hostname, gchar *msg, void (*callbackfunc)(gchar*));

/**
* \brief Connects to gappman to update the resolution for a program
* \param portno	portnumber gappman listens to. Note, this is actually not used when calling this function using the dbus version.
* \param hostname servername of host that runs gappman. Note, this is actually not used when calling this function using the dbus version.
* \param name name of the program for which the startup resolution should be changed.
* \param width screen-width in pixels
* \param height screen-height in pixels
* \return integer value as defined in libs/generic/gm_generic.h
*/
 
int gm_set_default_resolution_for_program(int portno, const char* hostname, const gchar* name, int width, int height);

#endif
