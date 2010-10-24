#include <dbus/dbus-glib.h>
#include <stdio.h>
#include <stdlib.h>

static void do_thread()
{
  GError *error = NULL;
  DBusGConnection *bus;
  gchar* args[] = { "-c", "1", "google.com", NULL };
  gint status, i;
  DBusGProxy *proxy;

  g_type_init ();

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
    bus = NULL;
  }

	for( i = 0; i < 80; i++)
	{	
  	if (!dbus_g_proxy_call (proxy, "RunCommand", &error,
   	     G_TYPE_STRING, "ping", G_TYPE_STRV, args, G_TYPE_INVALID,
   	     G_TYPE_INT, &status, G_TYPE_INVALID))
  	{
   	 g_warning ("Failed to complete RunCommand: %d, %s", i, error->message);
   	 g_error_free(error);
			error = NULL;
		 sleep(1);
  	}
		else
		{
			g_message("%d, received %d", i, status);
		}
	}
	return;	
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

  gtk_init (&argc, &argv);


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

	g_thread_join(thread);

	return 0;
}
