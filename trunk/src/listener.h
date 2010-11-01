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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined (HAVE_NETDB_H) && defined (HAVE_NETINET_IN_H) 
#include <gtk/gtk.h>

/**
* \brief Starts the gappman listener. Should be started only once.
* \return TRUE if setting up the channel succeeded. False otherwise.
*/
gboolean gappman_start_listener (GtkWidget *win);

/**
* \brief Closes a listener.
* \param *gio pointer to an open GIOChannel, if NULL it will close gappman's listener
* \return TRUE if closing the channel succeeded. False otherwise.
*/
gboolean gappman_close_listener (GIOChannel* gio);

/**
* \brief sets the configuration file location so it can be send to clients asking for it.
* \param *path string containing the absolute path to the configuration file for gappman.
*/
void gappman_set_confpath(const gchar *path);
#endif
