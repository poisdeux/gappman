/**
 * \file listener.c
 *
 *
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 * \bug relative confpath is not translated to absolute path. This makes some applets fail when not running under the same CWD.
 */

#if !defined(NO_LISTENER)
#include "listener.h"
#include <gm_generic.h>

#if defined(WITH_DBUS_SUPPORT)
#include "listener-dbus.h"
#else
#include "listener-socket.h"
#endif // WITH_DBUS_SUPPORT

gboolean gappman_start_listener(GtkWidget * win)
{
#if defined(WITH_DBUS_SUPPORT)
	return listener_dbus_start_session();
#else
	return listener_socket_open(win);
#endif // WITH_DBUS_SUPPORT)
}

gboolean gappman_close_listener()
{
#if defined(WITH_DBUS_SUPPORT)
	return GM_SUCCESS;
#else
	return listener_socket_close(NULL);
#endif // WITH_DBUS_SUPPORT
}
#endif // NO_LISTENER
