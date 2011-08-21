/**
 * \file gm_network.c
 *
 *
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 *
 * \todo add function gm_get_programs_from_gappman which provides a list of managed programs and URLs to the corresponding buttonimages
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
#include "gm_network.h"

#if defined(WITH_DBUS_SUPPORT) && !defined(NO_LISTENER)
#include "gm_network-dbus.h"
#elif !defined(NO_LISTENER)
#include "gm_network-socket.h"
#endif

void gm_network_free_proceslist(struct proceslist *procslist)
{
	struct proceslist *tmp;
	while (procslist != NULL)
	{
		tmp = procslist->prev;
		free(procslist);
		procslist = tmp;
	}
}

int gm_network_connect_to_gappman(int portno, const char *hostname, int *sockfd)
{
#if defined(NO_LISTENER) || defined(WITH_DBUS_SUPPORT)
	return GM_NET_COMM_NOT_SUPPORTED;
#else
	return gm_socket_connect_to_gappman(portno, hostname, sockfd);
#endif
}

int gm_network_get_started_procs_from_gappman(int portno, const char *hostname,
									  struct proceslist **startedprocs)
{
#ifdef NO_LISTENER
	return GM_NET_COMM_NOT_SUPPORTED;
#elif defined(WITH_DBUS_SUPPORT)
	return gm_dbus_get_started_procs_from_gappman(startedprocs);
#else
	return gm_socket_get_started_procs_from_gappman(portno, hostname,
													startedprocs);
#endif
}

GmReturnCode gm_network_get_confpath_from_gappman(int portno, const char *hostname,
								 gchar ** path)
{
#ifdef NO_LISTENER
	return GM_NET_COMM_NOT_SUPPORTED;
#elif defined(WITH_DBUS_SUPPORT)
	return gm_dbus_get_confpath_from_gappman(path);
#else
	return gm_socket_get_confpath_from_gappman(portno, hostname, path);
#endif
}

int gm_network_get_fontsize_from_gappman(int portno, const char *hostname,
								 int *fontsize)
{
#ifdef NO_LISTENER
	return GM_NET_COMM_NOT_SUPPORTED;
#elif defined(WITH_DBUS_SUPPORT)
	return gm_dbus_get_fontsize_from_gappman(fontsize);
#else
	return gm_socket_get_fontsize_from_gappman(portno, hostname, fontsize);
#endif
}

int gm_network_send_and_receive_message(int portno, const char *hostname, gchar * msg,
								void (*callbackfunc) (gchar *))
{
#if defined(WITH_DBUS_SUPPORT) || defined(NO_LISTENER)
	return GM_NET_COMM_NOT_SUPPORTED;
#else
	return gm_socket_send_and_receive_message(portno, hostname, msg,
											  callbackfunc);
#endif
}

int gm_network_set_default_resolution_for_program(int portno, const char *hostname,
										  const gchar * name, int width,
										  int height)
{

#if defined(NO_LISTENER)
	return GM_NET_COMM_NOT_SUPPORTED;
#elif defined(WITH_DBUS_SUPPORT)
	return gm_dbus_set_default_resolution_for_program(name, width, height);
#else
	return gm_socket_set_default_resolution_for_program(portno, hostname, name,
														width, height);
#endif
}

#if defined(DEBUG)
int gm_network_get_window_geometry_from_gappman(int portno, const char *hostname, int *width, int *height)
{
#ifdef NO_LISTENER
	return GM_NET_COMM_NOT_SUPPORTED;
#elif defined(WITH_DBUS_SUPPORT)
	return gm_dbus_get_window_geometry_from_gappman(width, height);
#else
	return gm_socket_get_window_geometry_from_gappman(portno, hostname, width, height);
#endif
}
#endif


