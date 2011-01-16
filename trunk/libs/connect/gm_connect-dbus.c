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
#include <dbus/dbus-glib.h>
#include <gm_generic.h>
#include "gm_connect.h"
#include "gm_connect-generic.h"

static GMutex *check_status_mutex;

static void get_lock()
{
  while( g_mutex_trylock(check_status_mutex) == FALSE )
  {
    sleep(1);
  }
}

static void release_lock()
{
  g_mutex_unlock(check_status_mutex);
}

int gm_dbus_get_started_procs_from_gappman(int portno, const char* hostname, struct proceslist **startedprocs)
{
  GError *error = NULL;
  gint foundname;
	gint n;
	gint i;
  DBusGProxyCall* proxy_call;
  DBusGConnection *bus;
  DBusGProxy *proxy;
	gchar **procs;
  gchar **contentssplit = NULL;
	
  //get_lock();

  bus = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
  if (bus == NULL)
  {
    g_warning ("Couldn't connect to session bus: %s\n", error->message);
    g_error_free(error);
    error = NULL;

    return FALSE;
  }

  proxy = dbus_g_proxy_new_for_name (bus,
               "gappman",
               "/GmAppman",
               "gappman.interface");

  if(proxy == NULL)
  {
    g_warning("Could not get dbus object for gappman.interface");
    dbus_g_connection_unref(bus);

    return FALSE;
  }

/*  proxy_call = dbus_g_proxy_call_with_timeout(proxy,
      "GetStartedProcs", 500, &error, 
			G_TYPE_INVALID, 
      G_TYPE_STRV, procs, G_TYPE_INVALID);
*/

  if (proxy_call == NULL)
  {
    g_warning ("Failed to call GetStartedProcs: %s", error->message);
    g_error_free(error);
    error = NULL;

    return FALSE;
  }

	for(n=0; procs[n] != NULL; n++)
	{	
		foundname = 0;
		contentssplit = g_strsplit(procs[n], "::", 0);
		for(i=0; contentssplit[i] != NULL; i++)
		{
    	if ( g_strcmp0("name", contentssplit[i]) == 0)
			{
				*startedprocs = createnewproceslist(*startedprocs);
				(*startedprocs)->name = contentssplit[i+1];
				foundname = 1;
			}
			else if ( g_strcmp0("pid", contentssplit[i]) == 0)
			{
				if ( foundname == 1 )
				{
					(*startedprocs)->pid = atoi(contentssplit[i+1]);
					foundname = 0;
				}
    	}
		}
	}
  //release_lock();

	return TRUE;
}

int gm_dbus_get_confpath_from_gappman(int portno, const char* hostname, gchar** path)
{
	return GM_FAIL;
}

int gm_dbus_get_fontsize_from_gappman(int portno, const char* hostname, int *fontsize)
{
	return GM_FAIL;
}
