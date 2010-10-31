#include <dbus/dbus-glib.h>
#include <stdio.h>
#include <stdlib.h>

static DBusGProxyCallNotify collect_status(DBusGProxy *proxy, DBusGProxyCall* proxy_call, void *data)
{
  GError *error = NULL;
  gint status;

  if( ! dbus_g_proxy_end_call(proxy, proxy_call, &error,
        G_TYPE_INT, &status, G_TYPE_INVALID) )
  {
    g_warning("Error retrieving ping status: %s", error->message);
    g_error_free(error);
  }
  else
  {
    g_message("Got status: %d", status);
  }
}

static gboolean check_status()
{
  GError *error = NULL;
  gchar* args[] = { "-c", "1", "google.com", NULL };
	gchar** re_args = NULL;
  gint status;
  DBusGProxyCall* proxy_call;
  DBusGConnection *bus;
  DBusGProxy *proxy;

	re_args = (char **) realloc(re_args, (11 * sizeof(char *)));

	re_args[0] = args[0];
	re_args[1] = args[1];
	re_args[2] = args[2];
	re_args[3] = NULL;

  bus = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
  if (bus == NULL)
  {
    g_warning ("Couldn't connect to session bus: %s\n", error->message);
    g_error_free(error);
    error = NULL;
    return FALSE;
  }

  proxy = dbus_g_proxy_new_for_name (bus,
               "gappman.netman",
               "/GmNetmand",
               "gappman.netman.NetmanInterface");

  if(proxy == NULL)
  {
    g_warning("Could not get dbus object for gappman.netman.NetmanInterface");
    dbus_g_connection_unref(bus);
    return FALSE;
  }

  proxy_call = dbus_g_proxy_begin_call(proxy,
      "RunCommand", (DBusGProxyCallNotify) collect_status, NULL, NULL,
      G_TYPE_STRING, "ping", G_TYPE_STRV, re_args, G_TYPE_INVALID);

  if (proxy_call == NULL)
  {
    g_warning ("Failed to call RunCommand: %s", error->message);
    g_error_free(error);
    error = NULL;
    return FALSE;
  }

  return TRUE;
}


static int do_thread()
{
	gint i;

	for(i = 0; i < 80; i++ )
	{
		gdk_threads_enter();
		check_status();
		gdk_threads_leave();
		sleep(1);
	}
}

int
main (int argc, char **argv)
{
  GError *error = NULL;
  guint i;
  gint status;
	GThread *thread;
  GOptionContext *context;
	gchar* command;
	gchar **args;
	GOptionEntry entries[] =
	{
  	{ "command", 'c', 0, G_OPTION_ARG_STRING, &command, "Command to be executed", "NAME" },
  	{ "arg", 'a', 0, G_OPTION_ARG_STRING_ARRAY, &args, "Argument to be passed to the command. Use multiple --arg (-a) to specify multiple arguments", "ARG" },
  	{ NULL }
	};

  //Needs to be called before any another glib function
  if (!g_thread_supported ())
  {
    g_thread_init (NULL);
    gdk_threads_init();
  }

	g_type_init();

  gtk_init (&argc, &argv);

  context = g_option_context_new ("- test gm_netmand daemon");
  g_option_context_add_main_entries (context, entries, NULL);
  //g_option_context_add_group (context, gtk_get_option_group (TRUE));

  if (!g_option_context_parse (context, &argc, &argv, &error))
  {
    g_print ("option parsing failed: %s\n", error->message);
		g_error_free(error);
		error = NULL;
    return 1;
  }

	thread = g_thread_create((GThreadFunc) do_thread, NULL, TRUE, NULL);
	if ( thread == NULL )
  {
  	g_warning("Failed to create thread");
		return 1;
  }
	
	gdk_threads_enter();
  gtk_main();
  gdk_threads_leave();
	
	return 0;
}
