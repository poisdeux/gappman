/***
 * \file listener.c
 *
 * 
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 */


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
#define UPDATE_RES 3

static GIOChannel* gio; 

static int parsemessage(gchar *msg)
{
	static int state = 0;
	int msg_id;
        gchar** contentssplit = NULL;
        int i = 0;
        contentssplit = g_strsplit(msg, "::", 0);

	g_debug("parsemessage: %s", contentssplit[1]);
	if(g_strcmp0(contentssplit[1], "listprocesses") == 0)
	{
		msg_id = SEND_PROCESS_LIST;
	}
	else if(g_strcmp0(contentssplit[1], "showfontsize") == 0)
	{
		msg_id = SEND_FONTSIZE;
	}
	else if(g_strcmp0(contentssplit[1], "updateres") == 0)
	{
		msg_id = UPDATE_RES;
	}
	return msg_id;	
}

static void writemsg(GIOChannel* gio, gchar* msg)
{
	GIOError gerror = G_IO_ERROR_NONE;
	gsize bytes_written;

	gerror = g_io_channel_write(gio, msg, strlen(msg), &bytes_written);
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
		if(strlen(appw_list->menu_elt->name) < 256)
		{
			g_sprintf(msg, "::name::%s", appw_list->menu_elt->name);
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

static void handle_update_resolution(gchar *msg)
{
        gchar** contentssplit = NULL;
	gchar* name = NULL;
        int i = 0;
	int width, height;

        contentssplit = g_strsplit(msg, "::", 5);

	//Msg should contain 4 elements as according to protocol
	for (i = 0; i < 5; i++)
	{
		if(contentssplit[i] == NULL)
			return;
	}

	if( g_strcmp0(contentssplit[2], "general") == 0 )
	{
		name = NULL;
	}
	else
	{
		name = contentssplit[2];
	}
	width = atoi(contentssplit[3]);
	height = atoi(contentssplit[4]);

	update_resolution(name, width, height);

	g_strfreev(contentssplit);
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
		g_debug("Error on accept.\n");
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

		status = g_io_channel_read_line(new_gio, &msg, &len, NULL,  &gerror);
		if( status == G_IO_STATUS_ERROR )
		{
			g_warning ("Listener (handleconnection): %s\n", gerror->message);
		}
		else
		{
			g_debug("Received: %s", msg);
			msg_id = parsemessage(msg);
			switch(msg_id)
			{
				case SEND_PROCESS_LIST:
					sendprocesslist(new_gio);
					break;;
				case SEND_FONTSIZE:
					g_free(msg);
					msg = (gchar*) malloc(16 * sizeof(gchar));
					g_sprintf(msg, "fontsize::%d", gm_get_fontsize());
					writemsg(new_gio, msg);
					break;;
				case UPDATE_RES:
					handle_update_resolution(msg);
					break;;
			}
			g_free(msg);
		}
		gappman_close_listener(new_gio);
	}
	//Always return TRUE to keep watch active.	
	return TRUE; 
}

gboolean gappman_start_listener (GtkWidget* win)
{
	int sock;
	int s;
	int sourceid;
	struct hostent *host;
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	struct sockaddr_in addr;
	const gchar *server = "localhost"; 
	const gchar *port = "2103";
	gboolean listener_started = TRUE;	

	g_debug("Starting listener: %s:%s", server, port);

	memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_INET;    /* Allow IPv4 or IPv6 */
        hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
        hints.ai_flags = 0;
        hints.ai_protocol = 0;          /* Any protocol */

	s = getaddrinfo(NULL, "2103", &hints, &result);
        if (s != 0) {
            g_warning("getaddrinfo: %s\n", gai_strerror(s));
		listener_started = FALSE;	
        }
	else
	{
		for (rp = result; rp != NULL; rp = rp->ai_next) 
		{
               		sock = socket(rp->ai_family, rp->ai_socktype,
                            rp->ai_protocol);

               		if (sock == -1)
                  		continue;

	               if (bind(sock, rp->ai_addr, rp->ai_addrlen) != -1)
            		       break;                  /* Success */

               		close(sock);
           	}
		if (rp == NULL) /* No address succeeded */
		{               
			g_debug("RP == NULL");	
			listener_started = FALSE;	
           	}

           	freeaddrinfo(result);           /* No longer needed */

		if( listener_started == TRUE )
		{		
			gio = g_io_channel_unix_new (sock);

			if(! g_io_add_watch( gio, G_IO_IN, handleconnection, (gpointer) sock ))
			{
				g_warning("Cannot add watch on GIOChannel!\n");
				listener_started = FALSE;	
			}
		}
	}
	
	if ( listener_started == TRUE )
	{	
		g_message("Listening on port %s on %s\n", port, server);
		return TRUE;
	}
	else
	{
		close(sock);
		g_warning("Could not start listener");
		gm_show_confirmation_dialog("Could not start listener.\nShould I try again?", "Restart listener", gappman_start_listener, win, "Cancel", NULL, NULL, win);
		return FALSE;
	}
}

gboolean gappman_close_listener (GIOChannel* close_gio)
{
	GIOStatus status;
	GError *gerror = NULL;
	if ( close_gio == NULL )
	{
		close_gio = gio;
	}
  if(close_gio == NULL)
  {  
    return FALSE;
  }

  status = g_io_channel_shutdown( close_gio, TRUE, &gerror);
  if( status == G_IO_STATUS_ERROR )
  {
    	g_warning("Listener (gappman_close_listener): %s\n", gerror->message);
	return FALSE;
  }

	return TRUE;
}
