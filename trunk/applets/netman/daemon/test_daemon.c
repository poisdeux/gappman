#include <dbus/dbus-glib.h>
#include <stdio.h>
#include <stdlib.h>

static void do_nothing_glibbish()
{
	int i;

	for ( i = 0; i < 80; i++)
	{
		printf("Look ma, no glib! %d\n", i);
	}
}

static void check_status()
{
  GError *error = NULL;
  DBusGConnection *bus;
	gint status;
  gchar* args[] = { "-c", "1", "google.com", NULL };
  DBusGProxy *proxy;


  bus = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
  if (bus == NULL)
  {
    g_warning ("Couldn't connect to session bus: %s\n", error->message);
    g_error_free(error);
		error = NULL;
    return;
  }

  proxy = dbus_g_proxy_new_for_name (bus,
               "gappman.netman",
               "/GmNetmand",
               "gappman.netman.NetmanInterface");

  if(proxy == NULL)
  {
    g_warning("Could not get dbus object for gappman.netman.NetmanInterface");
    dbus_g_connection_unref(bus);
		return;
  }

  	if (!dbus_g_proxy_call (proxy, "RunCommand", &error,
   	     G_TYPE_STRING, "ping", G_TYPE_STRV, args, G_TYPE_INVALID,
   	     G_TYPE_INT, &status, G_TYPE_INVALID))
  	{
   	 	g_warning ("Failed to complete RunCommand: %s", error->message);
   	 	g_error_free(error);
			error = NULL;
  	}
		else
		{
			g_message("received %d", status);
		}

  dbus_g_connection_unref(bus);
	return;	
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
	//thread = g_thread_create((GThreadFunc) do_nothing_glibbish, NULL, TRUE, NULL);
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
