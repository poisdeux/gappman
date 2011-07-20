/**
 * \file listener-dbus.c
 *
 *
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 */

#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <dbus/dbus-glib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "appmanager.h"
#include "listener-dbus.h"
#include <gm_listener_glue.h>

G_DEFINE_TYPE(GmAppmanager, gm_appmanager, G_TYPE_OBJECT);	///< will create
															// gm_appmanager
															// and set
															// gm_appmanager_parent_class

static const gchar *confpath = "";

gboolean send_confpath(GmAppmanager * obj, gchar ** path,
							  GError ** error)
{
	struct metadata *config;
	config = appmanager_get_metadata();
	*path = g_strdup(config->conffile);
	return TRUE;
}

gboolean send_window_geometry(GmAppmanager * obj, gint *width, gint *height,
							  GError ** error)
{
	struct metadata *config;
	config = appmanager_get_metadata();
	*width = config->window_width;
	*height = config->window_height;
	return TRUE;
}

gboolean send_fontsize(GmAppmanager * obj, gint * fontsize,
							  GError ** error)
{
	*fontsize = gm_get_fontsize();
	return TRUE;
}

gboolean send_proceslist(GmAppmanager * obj, gchar *** proceslist,
								GError ** error)
{
	struct process_info *started_apps;
	gchar *msg = NULL;
	int number_of_appws = 0;
	int i;

	*proceslist = NULL;

	// gdk_thread lock??


	started_apps = appmanager_get_started_apps();
	while (started_apps != NULL)
	{
		number_of_appws++;
		*proceslist =
			(gchar **) realloc(*proceslist,
							   (number_of_appws + 1) * sizeof(gchar *));
		if (*proceslist == NULL)
		{
			g_warning("Could not reallocate memory. errno: %d", errno);
			return FALSE;
		}

		msg = (gchar *) malloc((256) * sizeof(gchar));
		if (msg == NULL)
		{
			g_warning("Could not allocate memory. errno: %d", errno);
			return FALSE;
		}
		g_snprintf(msg, 256, "name::%s::pid::%d", started_apps->menu_elt->name,
				   started_apps->PID);
		(*proceslist)[number_of_appws - 1] = msg;
		started_apps = started_apps->prev;
	}

	if (number_of_appws > 0)
	{
		(*proceslist)[number_of_appws] = NULL;
	}

	// gdk_thread lock??

	return TRUE;
}

gboolean update_resolution(GmAppmanager * obj, gchar * name, gint width,
								  gint height, GError ** error)
{
	appmanager_update_resolution(name, width, height);
	return TRUE;
}

static void gm_appmanager_class_init(GmAppmanagerClass * klass)
{
}

static void gm_appmanager_init(GmAppmanager * self)
{
}


gboolean listener_dbus_start_session()
{
	DBusGConnection *bus;
	DBusGProxy *bus_proxy;
	GError *error = NULL;
	GmAppmanager *obj;
	guint request_name_result;

	// Install introspection information so we can invoke the methods by name
	dbus_g_object_type_install_info(GM_TYPE_APPMANAGER,
									&dbus_glib_gm_appmanager_object_info);

	bus = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
	if (!bus)
		g_error("Couldn't connect to session bus: %s", error->message);

	bus_proxy = dbus_g_proxy_new_for_name(bus, "org.freedesktop.DBus",
										  "/org/freedesktop/DBus",
										  "org.freedesktop.DBus");


	if (!dbus_g_proxy_call(bus_proxy, "RequestName", &error,
						   G_TYPE_STRING, "gappman.appmanager",
						   G_TYPE_UINT, DBUS_NAME_FLAG_DO_NOT_QUEUE,
						   G_TYPE_INVALID,
						   G_TYPE_UINT, &request_name_result, G_TYPE_INVALID))
	{
		g_warning("Failed to acquire gappman: %s", error->message);
	}

	obj = g_object_new(GM_TYPE_APPMANAGER, NULL);

	dbus_g_connection_register_g_object(bus, "/GmAppmanager", G_OBJECT(obj));

	return TRUE;
}
