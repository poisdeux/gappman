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


int
main (int argc, char **argv)
{
  DBusGConnection *bus;
  DBusGProxy *remote_object;
  GError *error = NULL;
  guint i;
	gchar *args[] = { "arg1", "arg2", NULL };
  gint status;

  g_type_init ();

  bus = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
  if (!bus)
    lose_gerror ("Couldn't connect to session bus", error);
  
  remote_object = dbus_g_proxy_new_for_name (bus,
					     "gappman.netman",
					     "/GmNetmand",
					     "gappman.netman.NetmanInterface");

  if (!dbus_g_proxy_call (remote_object, "RunCommand", &error,
			  G_TYPE_STRING, "echo", G_TYPE_STRV, args, G_TYPE_INVALID,
			  G_TYPE_INT, &status, G_TYPE_INVALID))
    lose_gerror ("Failed to complete RunCommand", error);


  g_object_unref (G_OBJECT (remote_object));

  exit(0);
}
