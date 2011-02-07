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

static DBusGProxy* get_proxy()
{
	GError *error = NULL;
  DBusGConnection *bus;
	DBusGProxy *proxy;

  bus = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
  if (bus == NULL)
  {
    g_warning ("Couldn't connect to session bus: %s\n", error->message);
    g_error_free(error);
    error = NULL;

    return FALSE;
  }

  proxy = dbus_g_proxy_new_for_name (bus,
               "gappman.appmanager",
               "/GmAppmanager",
               "gappman.appmanager.Interface");

  if(proxy == NULL)
  {
    g_warning("Could not get dbus object for gappman.interface");
    dbus_g_connection_unref(bus);
  }

	return proxy;
}

int gm_dbus_get_started_procs_from_gappman(gint portno, const char* hostname, struct proceslist **startedprocs)
{
  GError *error = NULL;
  gint foundname;
	gint n;
	gint i;
  gboolean status;
  DBusGProxy *proxy;
	gchar **procs = NULL;
  gchar **contentssplit = NULL;
	
  //get_lock();

	proxy = get_proxy();

	g_debug("Starting GetStartedProcs");
	status = dbus_g_proxy_call_with_timeout(proxy,
      "GetStartedProcs", 500, &error, 
			G_TYPE_INVALID, 
      G_TYPE_STRV, &procs, G_TYPE_INVALID);
	g_debug("Ended GetStartedProcs");

  if (status == FALSE)
  {
		g_debug("Failed to call GetStartedProcs");
    g_warning ("Failed to call GetStartedProcs: %s", error->message);
    g_error_free(error);
    error = NULL;

    return GM_FAIL;
  }

	g_debug("Starting creating proceslist struct: %p", procs);
	g_debug("procs[0]: %s", procs[0]);
	for(n=0; procs[n] != NULL; n++)
	{	
		g_debug("parsing %s", procs[n]);
		foundname = 0;
		contentssplit = g_strsplit(procs[n], "::", 0);
		for(i=0; contentssplit[i] != NULL; i++)
		{
			g_debug("%s", contentssplit[i]);
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
	g_debug("returning from gm_dbus_get_started_procs_from_gappman");
	return GM_SUCCES;
}

gint gm_dbus_get_confpath_from_gappman(gint portno, const char* hostname, gchar** path)
{
	return GM_FAIL;
}

gint gm_dbus_get_fontsize_from_gappman(gint portno, const char* hostname, gint *fontsize)
{
  GError *error = NULL;
	DBusGProxy *proxy;
	gboolean status;

	proxy = get_proxy();
  status = dbus_g_proxy_call_with_timeout(proxy,
      "GetFontsize", 500, &error, 
			G_TYPE_INVALID, 
      G_TYPE_INT, fontsize, G_TYPE_INVALID);

  if (status == FALSE)
  {
    g_warning ("Failed to call GetFontsize: %s", error->message);
    g_error_free(error);
    error = NULL;

		return GM_FAIL;
  }
	
	return GM_SUCCES;
}
