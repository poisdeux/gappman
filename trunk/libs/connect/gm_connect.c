/**
 * \file gm_connect.c
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
#include <gm_generic.h>
#include "gm_connect.h"

#if defined(WITH_DBUS_SUPPORT) && !defined(NO_LISTENER)
#include "gm_connect-dbus.h"
#elif !defined(NO_LISTENER)
#include "gm_connect-socket.h"
#endif

void gm_free_proceslist(struct proceslist *procslist)
{
	struct proceslist *tmp;
	while (procslist != NULL)
	{
		tmp = procslist->prev;
		free(procslist);
		procslist = tmp;
	}
}

int gm_connect_to_gappman(int portno, const char *hostname, int *sockfd)
{
#if defined(NO_LISTENER) || defined(WITH_DBUS_SUPPORT)
	return GM_NET_COMM_NOT_SUPPORTED;
#else
	return gm_socket_connect_to_gappman(portno, hostname, sockfd);
#endif
}

int gm_get_started_procs_from_gappman(int portno, const char *hostname,
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

int gm_get_confpath_from_gappman(int portno, const char *hostname,
								 gchar ** path)
{
#ifdef NO_LISTENER
	return GM_NET_COMM_NOT_SUPPORTED;
#elif defined(WITH_DBUS_SUPPORT)
	return gm_dbus_get_confpath_from_gappman(path);
#elif defined(WITH_DBUS_SUPPORT)
	return gm_dbus_get_confpath_from_gappman(path);
#else
	return gm_socket_get_confpath_from_gappman(portno, hostname, path);
#endif
}


int gm_get_fontsize_from_gappman(int portno, const char *hostname,
								 int *fontsize)
{
#ifdef NO_LISTENER
	return GM_NET_COMM_NOT_SUPPORTED;
#elif defined(WITH_DBUS_SUPPORT)
	return gm_dbus_get_fontsize_from_gappman(fontsize);
#elif defined(WITH_DBUS_SUPPORT)
	return gm_dbus_get_fontsize_from_gappman(fontsize);
#else
	return gm_socket_get_fontsize_from_gappman(portno, hostname, fontsize);
#endif
}

int gm_send_and_receive_message(int portno, const char *hostname, gchar * msg,
								void (*callbackfunc) (gchar *))
{
#if defined(WITH_DBUS_SUPPORT) || defined(NO_LISTENER)
	return GM_NET_COMM_NOT_SUPPORTED;
#else
	return gm_socket_send_and_receive_message(portno, hostname, msg,
											  callbackfunc);
#endif
}

int gm_set_default_resolution_for_program(int portno, const char *hostname,
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
