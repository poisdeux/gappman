/**
 * \file gm_netmand.c
 *
 *
 *
 * GPL v2
 *
 * \author Martijn Brekhof <m.brekhof@gmail.com>
 */
#include <stdlib.h>
#include <dbus/dbus-glib.h>
#include "gm_netmand.h"

G_DEFINE_TYPE (GmNetmand, gm_netmand, G_TYPE_OBJECT); ///< will create gm_netmand_get_type and set gm_netmand_parent_class

static gboolean gm_netmand_run_command(GmNetmand *obj, gchar* command, gchar** args, GError **error)
{
	g_debug("gm_netmand_run_command called");
	g_debug("command: %s", command);
	for (; *args != NULL; args++)
		g_debug("arg: %s", *args);

	return TRUE;
}

static void restart_network(gchar *interface)
{
	g_debug("restart_network: %s", interface);
}


static void gm_netmand_class_init (GmNetmandClass *klass)
{
}

static void gm_netmand_init (GmNetmand *self)
{
}

/**
* \brief starts the mainloop and starts a D-Bus session
*/
int main ()
{
  DBusGConnection *bus;
  DBusGProxy *bus_proxy;
  GError *error = NULL;
  GmNetmand *obj;
  GMainLoop *mainloop;
  guint request_name_result;

  g_type_init ();

  //Install introspection information so we can invoke the methods by name
  dbus_g_object_type_install_info (GM_TYPE_NETMAND, &dbus_glib_gm_netmand_object_info);

  mainloop = g_main_loop_new (NULL, FALSE);

  bus = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
  if (!bus)
    g_error("Couldn't connect to session bus: %s", error->message);

  bus_proxy = dbus_g_proxy_new_for_name (bus, "org.freedesktop.DBus",
           "/org/freedesktop/DBus",
           "org.freedesktop.DBus");


  if (!dbus_g_proxy_call (bus_proxy, "RequestName", &error,
        G_TYPE_STRING, "gappman.netman",
        G_TYPE_UINT, DBUS_NAME_FLAG_DO_NOT_QUEUE,
        G_TYPE_INVALID,
        G_TYPE_UINT, &request_name_result,
        G_TYPE_INVALID))
	{
    g_warning("Failed to acquire gappman.netman: %s", error->message);
	}

  obj = g_object_new (GM_TYPE_NETMAND, NULL);

  dbus_g_connection_register_g_object (bus, "/GmNetmand", G_OBJECT (obj));

  g_message("gm_netmand running\n");

  g_main_loop_run (mainloop);

  exit (0);
}

