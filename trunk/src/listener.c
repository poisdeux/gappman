#include <stdio.h>
#include <glib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

gboolean handlemessage( GIOChannel* gio , GIOCondition cond, gpointer data )
{
	gsize len;
	gchar *msg;
	int bytes_read;
	GError *gerror = NULL;
	GIOStatus status = G_IO_STATUS_NORMAL;
	int newsock;
	struct sockaddr_in cli_addr;
	int clilen;
	gchar line[] = { ';', '\n'};
  GIOChannel* new_gio = NULL;

	newsock = accept((int) data, (struct sockaddr *) &cli_addr, &clilen);
	if(newsock < 0)
	{
		printf("Error on accept.\n");
		// we return TRUE to keep watch active.
		return TRUE;
	}

	if (cond & G_IO_IN)
	{
		new_gio = g_io_channel_unix_new (newsock);
		g_io_channel_set_buffer_size (new_gio, 0);
		g_io_channel_set_encoding (new_gio, "UTF-8", NULL);
		g_io_channel_set_line_term (new_gio, line, 2);

		printf("DEBUG: handlemessage\n");
		fflush(stdout);

		status = g_io_channel_read_line(new_gio, &msg, &len, NULL,  &gerror);
		if( status == G_IO_STATUS_ERROR )
		{
			g_error ("Listener (handlemessage): %s\n", gerror->message);
		}
		else
		{
			fprintf(stdout, "MESSAGE RECEIVED: %s (%d)\n", msg, len);
			fflush(stdout);
			g_free(msg);
		}

  	status = g_io_channel_shutdown( new_gio, TRUE, &gerror);
		if ( status == G_IO_STATUS_ERROR )
			g_error ("Listener (handlemessage): %s\n", gerror->message);
	}
	//Always return TRUE to keep watch active.	
	return TRUE; 
}

gboolean gappman_start_listener (GIOChannel* gio, const gchar *server, gint port)
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

	if(! g_io_add_watch( gio, G_IO_IN, handlemessage, (gpointer) sock ))
	{
		fprintf(stderr, "Cannot add watch on GIOChannel!\n");
	}
  
	return TRUE;
}

gboolean gappman_close_listener (GIOChannel* gio)
{
	GIOStatus status;
	GError *gerror = NULL;

  status = g_io_channel_shutdown( gio, TRUE, &gerror);
    if( status == G_IO_STATUS_ERROR )
    {
      g_error ("Listener (handlemessage): %s\n", gerror->message);
			return FALSE;
    }


	return TRUE;
}
