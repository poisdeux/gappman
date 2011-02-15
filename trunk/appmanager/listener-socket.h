/**
 * \file listener-socket.h
 *
 *
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>

/**
* \brief Starts the gappman listener. Should be started only once.
* \return TRUE if setting up the channel succeeded. False otherwise.
*/
gboolean listener_socket_open(GtkWidget * win);

/**
* \brief Closes a listener.
* \param close_gio pointer to an open GIOChannel, if NULL it will close gappman's listener
* \return TRUE if closing the channel succeeded. False otherwise.
*/
gboolean listener_socket_close(GIOChannel * close_gio);

/**
* \brief sets the configuration file location so it can be send to clients asking for it.
* \param *path string containing the absolute path to the configuration file for gappman.
*/
void listener_socket_set_confpath(const gchar * path);
