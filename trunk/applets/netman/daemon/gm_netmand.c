/**
 * \file gm_netmand.c
 *
 *
 *
 * GPL v2
 *
 * \author Martijn Brekhof <m.brekhof@gmail.com>
 */
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <dbus/dbus-glib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "gm_netmand.h"
#include <../generic/gm_netman_generic.h>

G_DEFINE_TYPE (GmNetmand, gm_netmand, G_TYPE_OBJECT); ///< will create gm_netmand_get_type and set gm_netmand_parent_class

static void test_print_args(gchar **args)
{
  int i;

  for ( i = 0; args[i] != NULL; i++ )
  {
    g_debug("arg: %s", args[i]);
  }
}

static gboolean gm_netmand_run_command(GmNetmand *obj, gchar* command, gchar** args, gint *exitcode, GError **error)
{
	int childpid;
	int tmp_exitcode;
	int index = 0;
	int timeout = 0;
  gchar **tmp;

  //note: we make the tmp array 2 bigger than sizeof(args) as we need to make 
  //the array null-terminated
	tmp = (gchar**) malloc ((sizeof(args) + 2 )*sizeof(gchar*));

	if (tmp == NULL)
		return FALSE;

	//first elt in the arg-list should be the command (needed by execvp)
	tmp[index++] = command;

	//fill rest of the tmp array with the elements from args
	for (; *args != NULL; args++)
	{	
		tmp[index++] = *args;
	}
	//Array must be NULL-terminated
	tmp[index] = (char *) NULL;
		
	childpid = fork();
  if ( childpid == 0 )
  {
  	if ( execvp((char *) command, tmp) == -1 )
  	{
  		g_warning("Could not execute %s: errno: %d\n", command, errno);
			g_set_error(error, GM_NETMAND_ERROR, GM_NETMAND_FAILED_EXEC, 
				"Failed to execute %s", command); 
  		_exit(1);
  	}	
  	_exit(0);
  }
  else if ( childpid < 0 )
  {
  	g_warning("Failed to fork!\n");
		g_set_error(error, GM_NETMAND_ERROR, GM_NETMAND_FAILED_EXEC, 
				"Failed to execute %s", command); 
  	return FALSE;
  }

	timeout=10;
	tmp_exitcode=-1;
	do
	{
		if( waitpid(childpid, &tmp_exitcode, WNOHANG) == -1 )
		{
			g_warning("Waitpid failed: %d", errno);
			g_set_error(error, GM_NETMAND_ERROR, GM_NETMAND_FAILED_WAIT, 
				"Failed while waiting for child: %d", childpid);  
			return FALSE;
		}
		sleep(1);
		timeout--;
	//Continue loop untill either child exited or child timed out.
	} while ((! WIFEXITED(tmp_exitcode)) && (timeout != 0));

	if(timeout == 0)
	{
		// should be replaced by a generic gm kill function (see src/appmanager.c for implementation)
		// to prevent a stall when child refuses to die
		kill(childpid, 15);
		waitpid(childpid, &tmp_exitcode, 0);
	}

	free(tmp);
	*exitcode = WEXITSTATUS(tmp_exitcode);
	return TRUE;
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

  g_main_loop_run (mainloop);

  exit (0);
}

