#include <glib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
//#include <stdlib.h>
//#include <string.h>
//#include <unistd.h>
//#include <wait.h>
//#include <signal.h>

void handlemessage( GIOChannel* gio )
{
/*
	GIOStatus           g_io_channel_read_chars             (GIOChannel *channel,
                                                         gchar *buf,
                                                         gsize count,
                                                         gsize *bytes_read,
                                                         GError **error);
*/

}

gboolean gappman_connect (const gchar *server, gint port)
{

	GIOChannel* gio;
	
	int sock;
	int sourceid;
	struct hostent *host;
	gchar line[] = { ';', '\n'};
	g_return_val_if_fail (server != NULL, FALSE);
	g_return_val_if_fail (port > 0, FALSE);
	if ((sock = socket (AF_INET, SOCK_STREAM, 0)) != -1)
	{
		struct sockaddr_in addr;
		host = gethostbyname (server);
		if (!host)
			return FALSE;
		addr.sin_addr = * (struct in_addr *) host->h_addr;
		addr.sin_port = g_htons(port);
		addr.sin_family = AF_INET;
		if (connect (sock, (struct sockaddr *) &addr, sizeof (addr)) == -1)
			return FALSE;


	}
	else
		return FALSE;
	gio = g_io_channel_unix_new (sock);
	g_io_channel_set_buffer_size (gio, 0);
	g_io_channel_set_line_term (gio, line, 2);
	g_io_channel_set_encoding (gio, "UTF-8", NULL);

	sourceid = g_io_add_watch_full( gio, G_PRIORITY_LOW, G_IO_IN, handlemessage, gio, NULL);
	
	return TRUE;
}

