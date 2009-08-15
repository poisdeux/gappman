#include <stdio.h>
#include <glib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

static GIOChannel* gio = NULL;

gboolean handlemessage( GIOChannel* gio , GIOCondition cond, gpointer data )
{
	gchar **msg;
	int bytes_read;
	gsize len;
	GError *gerror = NULL;
	GIOStatus status;

	if (cond & G_IO_IN)
	{
		printf("DEBUG: handlemessage\n");
		fflush(stdout);
		//return TRUE;
		status = g_io_channel_read_line(gio, msg, &len, NULL,  &gerror);
		if( status == G_IO_STATUS_ERROR )
		{
			g_error ("handlemessage: %s\n", gerror->message);
		}
		else
		{
			fprintf(stdout, "MESSAGE RECEIVED: %s\n", msg);
			g_free (msg);
		}
	}
	//Always return TRUE to keep watch active.	
	return TRUE; 
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
		if ( bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) 
			return FALSE;

		if ( listen(sock,5) != 0 )
			return FALSE;
		
		fprintf(stdout, "Listening to port %d on %s\n", port, server);
	}
	else
	{
		return FALSE;
	}

	gio = g_io_channel_unix_new (sock);
	g_io_channel_set_buffer_size (gio, 0);
	g_io_channel_set_line_term (gio, line, 2);
	g_io_channel_set_encoding (gio, "UTF-8", NULL);

	if(! g_io_add_watch( gio, G_IO_IN | G_IO_HUP, handlemessage, NULL ))
	{
		fprintf(stderr, "Cannot add watch on GIOChannel!\n");
	}

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
