#include <glib.h>

/**
* \brief Starts the gappman listener. Should be started only once. 
* \param **gio call by reference to a GIOChannel
* \param *server server address
* \param port portnumber gappman should listen on
* \return TRUE if setting up the channel succeeded. False otherwise.
*/ 
gboolean gappman_start_listener (GIOChannel** gio, const gchar *server, gint port);

/**
* \brief Closes the gappman listener.
* \param *gio pointer to an open GIOChannel
* \return TRUE if closing the channel succeeded. False otherwise.
*/
gboolean gappman_close_listener (GIOChannel* gio);

