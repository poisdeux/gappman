#include <stdio.h>
#include <glib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

static GIOChannel* gio = NULL;

gboolean handlemessage( GIOChannel* gio , GIOCondition cond, gpointer data )
{
	gchar msg[1];
	int bytes_read;
	GError *gerror = NULL;
	GIOStatus status;

	status = g_io_channel_read_chars(gio, msg, 1, &bytes_read, &gerror);
	if( gerror != NULL )
	{
		fprintf(stderr, "Error: %s\n", gerror->message );
	}
	if( status == G_IO_STATUS_NORMAL )
	{
		fprintf(stdout, "MESSAGE RECEIVED: %s\n", msg);
		return TRUE;
	}
	
	return FALSE; 
}

gboolean gappman_start_listener (const gchar *server, gint port)
{
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
		fprintf(stdout, "Listening to port %d on %s\n", port, server);
		if ( bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) 
			return FALSE;

		listen(sock,5);
	}
	else
	{
		return FALSE;
	}

	gio = g_io_channel_unix_new (sock);
	g_io_channel_set_buffer_size (gio, 0);
	g_io_channel_set_line_term (gio, line, 2);
	g_io_channel_set_encoding (gio, "UTF-8", NULL);

	sourceid = g_io_add_watch_full( gio, G_PRIORITY_LOW, G_IO_IN, handlemessage, gio, NULL);
	
	return TRUE;
}

gboolean gappman_close_listener ()
{
	GIOStatus status;

  status = g_io_channel_shutdown( gio, TRUE, NULL);

	if ( status == G_IO_STATUS_ERROR )
		return FALSE;

	return TRUE;
}
