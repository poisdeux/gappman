#include <stdio.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <gm_layout.h>
#include "appmanager.h"
#include "listener.h"

#define SEND_PROCESS_LIST 1
#define SEND_FONTSIZE 2

static int parsemessage(gchar *msg)
{
	static int state = 0;
	int msg_id;

	printf("DEBUG: state = %d\n", state);

	if(g_strcmp0(msg, "listprocesses\n") == 0)
	{
		msg_id = SEND_PROCESS_LIST;
	}
	else if(g_strcmp0(msg, "showfontsize\n") == 0)
	{
		msg_id = SEND_FONTSIZE;
	}
	return msg_id;	
}

static void writemsg(GIOChannel* gio, gchar* msg)
{
	GIOError gerror = G_IO_ERROR_NONE;
	gsize bytes_written;

	g_printf("Sending message: %s\n", msg);
	fflush(stdout);

	gerror = g_io_channel_write(gio, msg, strlen(msg), &bytes_written);
	g_printf("Send message: %s\n", msg);
	fflush(stdout);
	if( gerror != G_IO_ERROR_NONE )
	{
		g_warning("Error sending message: %s : bytes written %d\n", msg, bytes_written);
	}
}

static void sendprocesslist(GIOChannel* gio)
{
	struct appwidgetinfo* appw_list;
	gchar* msg = NULL;
	msg = (gchar*) malloc((256 + 16) * sizeof(gchar));
	
	appw_list = get_started_apps();
	while(appw_list != NULL)
	{
		g_printf("Name: %s, PID: %d\n", appw_list->name, appw_list->PID);
		if(strlen(appw_list->name) < 256)
		{
			g_sprintf(msg, "::name::%s", appw_list->name);
		}
		else
		{
			g_sprintf(msg, "::name::");
		}	
		writemsg(gio, msg);

		g_sprintf(msg, "::pid::%d", appw_list->PID);
		writemsg(gio, msg);
	
		appw_list = appw_list->prev;
	}

	free(msg);
}

static void answerclient(GIOChannel* gio, int msg_id)
{
	gsize* bytes_written = NULL;
	gchar* msg = NULL;

	switch(msg_id)
	{
		case SEND_PROCESS_LIST:
			sendprocesslist(gio);
			break;;
		case SEND_FONTSIZE:
			msg = (gchar*) malloc(16 * sizeof(gchar));
			g_sprintf(msg, "fontsize::%d", gm_get_fontsize());
			writemsg(gio, msg);
			free(msg);
			break;;
	}
}

static gboolean handleconnection( GIOChannel* gio , GIOCondition cond, gpointer data )
{
	gsize len;
	gchar *msg;
	int bytes_read;
	GError *gerror = NULL;
	GIOStatus status = G_IO_STATUS_NORMAL;
	int newsock;
	struct sockaddr_in cli_addr;
	int clilen;
  GIOChannel* new_gio = NULL;
	int msg_id;

	newsock = accept((int) data, (struct sockaddr *) &cli_addr, &clilen);
	if(newsock < 0)
	{
		printf("Error on accept.\n");
		// we return TRUE to keep watch active.
		return TRUE;
	}

	if (cond & G_IO_IN)
	{
		// TODO: fork to handle connections concurrently
		new_gio = g_io_channel_unix_new (newsock);
		g_io_channel_set_buffer_size (new_gio, 0);
		g_io_channel_set_encoding (new_gio, "UTF-8", NULL);
		g_io_channel_set_line_term (new_gio, NULL, -1);

		printf("DEBUG: handleconnection\n");
		fflush(stdout);

		status = g_io_channel_read_line(new_gio, &msg, &len, NULL,  &gerror);
		if( status == G_IO_STATUS_ERROR )
		{
			g_error ("Listener (handleconnection): %s\n", gerror->message);
		}
		else
		{
			fprintf(stdout, "MESSAGE RECEIVED: %s (%d)\n", msg, len);
			fflush(stdout);
			msg_id = parsemessage(msg);
			g_free(msg);
			answerclient(new_gio, msg_id);
		}
		gappman_close_listener(new_gio);
	}
	//Always return TRUE to keep watch active.	
	return TRUE; 
}

gboolean gappman_start_listener (GIOChannel** gio, const gchar *server, gint port)
{
	int sock;
	int sourceid;
	struct hostent *host;

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

	*gio = g_io_channel_unix_new (sock);

	if(! g_io_add_watch( *gio, G_IO_IN, handleconnection, (gpointer) sock ))
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
    g_error ("Listener (gappman_close_listener): %s\n", gerror->message);
		return FALSE;
  }

	return TRUE;
}
