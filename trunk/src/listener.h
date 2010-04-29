/***
 * \file listener.h
 *
 * 
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 */


#include <glib.h>

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

