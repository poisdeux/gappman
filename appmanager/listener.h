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

#ifndef __LISTENER_H__
#define __LISTENER_H__

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if !defined (NO_LISTENER)
#include <gtk/gtk.h>

/**
* \brief Starts the gappman listener. Should be started only once.
* \return TRUE if setting up the channel succeeded. False otherwise.
*/
gboolean gappman_start_listener(GtkWidget * win);

/**
* \brief Closes a listener.
* \return TRUE if closing the channel succeeded. False otherwise.
*/
gboolean gappman_close_listener();

#endif /* NO_LISTENER */
#endif /* __LISTENER_H__ */
