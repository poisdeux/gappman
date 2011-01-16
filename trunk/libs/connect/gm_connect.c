/**
 * \file gm_connect.c
 *
 *
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <sys/socket.h>
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <glib.h>
#include <string.h>
#include <gm_generic.h>
#include "gm_connect.h"


void freeproceslist(struct proceslist* procslist)
{
    struct proceslist* tmp;
    while (procslist != NULL)
    {
        tmp = procslist->prev;
        free(procslist);
        procslist = tmp;
    }
}

int gm_connect_to_gappman(int portno, const char* hostname, int *sockfd)
{
#ifdef NO_LISTENER
  return GM_NET_COMM_NOT_SUPPORTED;
#else
	return gm_socket_connect_to_gappman(portno, hostname, sockfd);
#endif
}

int gm_get_started_procs_from_gappman(int portno, const char* hostname, struct proceslist **startedprocs)
{
#ifdef NO_LISTENER
	return GM_NET_COMM_NOT_SUPPORTED;
#elif defined(WITH_DBUS_SUPPORT)
	return gm_dbus_get_started_procs_from_gappman(portno,hostname, startedprocs);
#else
	return gm_socket_get_started_procs_from_gappman(portno,hostname, startedprocs);
#endif
}

int gm_get_confpath_from_gappman(int portno, const char* hostname, gchar** path)
{
#ifdef NO_LISTENER
    return GM_NET_COMM_NOT_SUPPORTED;
#elif defined(WITH_DBUS_SUPPORT)
	return gm_dbus_get_confpath_from_gappman( portno, hostname, path);
#else
	return gm_socket_get_confpath_from_gappman( portno, hostname, path);
#endif
}


int gm_get_fontsize_from_gappman(int portno, const char* hostname, int *fontsize)
{
#ifdef NO_LISTENER
    return GM_NET_COMM_NOT_SUPPORTED;
#elif defined(WITH_DBUS_SUPPORT)
	return gm_dbus_get_fontsize_from_gappman(portno, hostname, fontsize);
#else
	return gm_socket_get_fontsize_from_gappman(portno, hostname, fontsize);
#endif
}

int gm_send_and_receive_message(int portno, const char* hostname, gchar *msg, void (*callbackfunc)(gchar*))
{
#ifdef NO_LISTENER | WITH_DBUS_SUPPORT
    return GM_NET_COMM_NOT_SUPPORTED;
#else
	return gm_socket_send_and_receive_message(portno, hostname, msg, callbackfunc);    
#endif
}
