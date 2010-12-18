/**
 * \file listener.c
 *
 *
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 */

#include "listener.h"

#if !defined(NO_LISTENER)

#if defined(WITH_DBUS_SUPPORT)
#include "listener-dbus.h"
#else
#include "listener-socket.h"
#endif //WITH_DBUS_SUPPORT

gboolean gappman_start_listener (GtkWidget* win)
{
#if defined(WITH_DBUS_SUPPORT)
	return listener_dbus_start_session(win);
#else
	return listener_socket_open(win);
#endif //WITH_DBUS_SUPPORT)	
}

gboolean gappman_close_listener ()
{
#if !defined(WITH_DBUS_SUPPORT)
	return listener_socket_close(NULL);
#endif //WITH_DBUS_SUPPORT)	
}

void gappman_set_confpath(const gchar *path)
{
#if defined(WITH_DBUS_SUPPORT)
	listener_dbus_set_confpath(path);
#else
	listener_socket_set_confpath(path);
#endif //WITH_DBUS_SUPPORT)	
}
#endif //NO_LISTENER
