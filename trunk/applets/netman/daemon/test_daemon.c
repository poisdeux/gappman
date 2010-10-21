#include <dbus/dbus-glib.h>
#include <stdio.h>
#include <stdlib.h>

static void lose (const char *fmt, ...) G_GNUC_NORETURN G_GNUC_PRINTF (1, 2);
static void lose_gerror (const char *prefix, GError *error) G_GNUC_NORETURN;


static void
lose (const char *str, ...)
{
  va_list args;

  va_start (args, str);

  vfprintf (stderr, str, args);
  fputc ('\n', stderr);

  va_end (args);

  exit (1);
}

static void
lose_gerror (const char *prefix, GError *error) 
{
  lose ("%s: %s", prefix, error->message);
}

static DBusGConnection* dbus_connect(DBusGProxy **proxy)
{
  GError *error = NULL;
  DBusGConnection *bus;

  bus = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
  if (bus == NULL)
  {
    g_warning ("Couldn't connect to session bus: %s\n", error->message);
    g_error_free(error);
    return bus;
  }

  *proxy = dbus_g_proxy_new_for_name (bus,
               "gappman.netman",
               "/GmNetmand",
               "gappman.netman.NetmanInterface");

  if(*proxy == NULL)
  {
    g_warning("Could not get dbus object for gappman.netman.NetmanInterface");
    dbus_g_connection_unref(bus);
    bus = NULL;
  }
  return bus;
}

static void dbus_disconnect(DBusGConnection *bus, DBusGProxy *proxy)
{
    if (bus) {
    dbus_g_connection_unref (bus);
  }

  if (proxy) {
    g_object_unref (proxy);
  }

}

static gboolean check_status()
{
  GError *error = NULL;
  gchar* args[] = { "-c", "1", "google.com", NULL };
  gint status;
  DBusGConnection *bus;
  DBusGProxy *remote_object;

  bus = dbus_connect(&remote_object);

  if( ! bus )
  {
    return FALSE;
  }

  if (!dbus_g_proxy_call (remote_object, "RunCommand", &error,
        G_TYPE_STRING, "ping", G_TYPE_STRV, args, G_TYPE_INVALID,
        G_TYPE_INT, &status, G_TYPE_INVALID))
  {
    g_warning ("Failed to complete RunCommand: %p: %s", remote_object, error->message);
    g_error_free(error);
    dbus_disconnect(bus, remote_object);
    return FALSE;
  }

  g_debug("status: %d", status);

  dbus_disconnect(bus, remote_object);

  return TRUE;
}

static void do_thread()
{
	while(TRUE)
	{
		check_status();
		sleep(1);
	}
}

int
main (int argc, char **argv)
{
  DBusGConnection *bus;
  DBusGProxy *remote_object;
  GError *error = NULL;
  guint i;
  gint status;
  GOptionContext *context;
	gchar* command;
	gchar **args;
	GOptionEntry entries[] =
	{
  	{ "command", 'c', 0, G_OPTION_ARG_STRING, &command, "Command to be executed", "NAME" },
  	{ "arg", 'a', 0, G_OPTION_ARG_STRING_ARRAY, &args, "Argument to be passed to the command. Use multiple --arg (-a) to specify multiple arguments", "ARG" },
  	{ NULL }
	};

  gtk_init (&argc, &argv);

  g_type_init ();

    //Needs to be called before any another glib function
    if (!g_thread_supported ())
    {
        g_thread_init (NULL);
        gdk_threads_init();
    }


  context = g_option_context_new ("- test gm_netmand daemon");
  g_option_context_add_main_entries (context, entries, NULL);
  //g_option_context_add_group (context, gtk_get_option_group (TRUE));

  if (!g_option_context_parse (context, &argc, &argv, &error))
  {
    g_print ("option parsing failed: %s\n", error->message);
    exit (1);
  }


	if (!g_thread_create((GThreadFunc) do_thread, NULL, FALSE, NULL))
            {
                g_warning("Failed to create thread");
            }

    gdk_threads_enter();
    gtk_main ();
    gdk_threads_leave();

  exit(0);
}
