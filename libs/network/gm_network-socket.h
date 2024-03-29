/**
 * \file gm_network-socket.h
 * \brief generic functions to retrieve or sent information from or to gappman
 *
 *
 * GPL v2
 *
 * \author
 *   Martijn Brekhof <m.brekhof@gmail.com>
 *
*/

#ifndef __GAPPMAN_CONNECT_H__
#define __GAPPMAN_CONNECT_H__

#include <gm_generic.h>

/**
* \brief struct to hold the process ID and program name retrieved from Gappman
*/
struct proceslist
{
	int pid;					///< process ID of proces started by gappman
	gchar *name;				///< programname as known by gappman
	struct proceslist *prev;	///< pointer to previous proces in proceslist
};

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
int gm_get_started_procs_from_gappman(int portno, const char *hostname,
									  struct proceslist **startedprocs);

/**
* \brief Connects to gappman and requests the fontsize
* \param portno portnumber gappman listens to
* \param hostname servername of host that runs gappman
* \param path pointer to a string holding the configuration path
* \return integer value (GM_*) as defined in libs/generic/gm_generic.h
*/
GmReturnCode gm_get_confpath_from_gappman(int portno, const char *hostname,
								 gchar ** path);


/**
* \brief Connects to gappman and requests the fontsize
* \param portno portnumber gappman listens to
* \param hostname servername of host that runs gappman
* \param fontsize adres of the pointer to an integer value (call by reference)
* \return integer value (GM_*) as defined in libs/generic/gm_generic.h
*/
int gm_get_fontsize_from_gappman(int portno, const char *hostname,
								 int *fontsize);

/**
* \brief Connects to gappman
* \param portno portnumber gappman listens to
* \param hostname servername of host that runs gappman
* \param sockfd pointer to int which will hold the socket filedescriptor
* \return filedescriptor
*/
int gm_connect_to_gappman(int portno, const char *hostname, int *sockfd);

/**
* \brief connects to gappman and sends a message and may receive one or more answers.
* \param portno portnumber gappman listens to
* \param hostname servername of host that runs gappman
* \param msg the message that should be sent to gappman
* \param callbackfunc callback function that should handle each message received from gappman. If NULL no messages will be received.
* \return integer value as defined in libs/generic/gm_generic.h
*/
int gm_socket_send_and_receive_message(int portno, const char *hostname,
									   gchar * msg,
									   void (*callbackfunc) (gchar *));

#if defined(DEBUG)
/**
* \brief Connects to gappman and requests gappman's main window height and width
* \param portno portnumber gappman listens to.
* \param width width of the window in pixels
* \param height height of the window in pixels
* \return integer value (GM_*) as defined in libs/generic/gm_generic.h
*/
int gm_socket_get_window_geometry_from_gappman(int portno, const char *hostname, int *width, int *height);
#endif // DEBUG

#endif // __GAPPMAN_CONNECT_H__
