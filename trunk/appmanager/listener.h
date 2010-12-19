/**
 * \file listener.h
 *
 *
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 */

#ifndef __LISTENER_DBUS_H__
#define __LISTENER_DBUS_H__

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if !defined (NO_LISTENER)
#include <gtk/gtk.h>

/**
* \brief Starts the gappman listener. Should be started only once.
* \return TRUE if setting up the channel succeeded. False otherwise.
*/
gboolean gappman_start_listener (GtkWidget *win);

/**
* \brief Closes a listener.
* \return TRUE if closing the channel succeeded. False otherwise.
*/
gboolean gappman_close_listener ();

/**
* \brief sets the configuration file location so it can be send to clients asking for it.
* \param *path string containing the absolute path to the configuration file for gappman.
*/
void gappman_set_confpath(const gchar *path);
#endif /* NO_LISTENER */
#endif /* __LISTENER_DBUS_H__ */ 
