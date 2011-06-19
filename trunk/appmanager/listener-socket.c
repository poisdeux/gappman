/**
 * \file listener-socket.c
 *
 *
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 */

#include "listener-socket.h"

#include <stdio.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <gm_layout.h>
#include "appmanager.h"

#define SEND_PROCESS_LIST 1		///< message id used to specify we received a
								// request to sent the current list of
								// programs started by gappman
#define SEND_FONTSIZE 2			///< message id used to specify we received a
								// request to sent the current default
								// fontsize used by gappman
#define UPDATE_RES 3			///< message id used to specify we received
								// an resolution update request
#define SEND_CONFPATH 4			///< message id used to specify we received a
								// request to sent the configuration path
								// gappman uses

static GIOChannel *gio;
static const gchar *confpath = "";
static const gchar *rcpath = "";

static int parsemessage(gchar * msg)
{
	int msg_id;
	gchar **contentssplit = NULL;
	contentssplit = g_strsplit(msg, "::", 0);

	if (g_strcmp0(contentssplit[1], "listprocesses") == 0)
	{
		msg_id = SEND_PROCESS_LIST;
	}
	else if (g_strcmp0(contentssplit[1], "showfontsize") == 0)
	{
		msg_id = SEND_FONTSIZE;
	}
	else if (g_strcmp0(contentssplit[1], "updateres") == 0)
	{
		msg_id = UPDATE_RES;
	}
	else if (g_strcmp0(contentssplit[1], "showconfpath") == 0)
	{
		msg_id = SEND_CONFPATH;
	}
	return msg_id;
}

static void writemsg(GIOChannel * gio, gchar * msg)
{
	GIOError gerror = G_IO_ERROR_NONE;
	gsize bytes_written;

	gerror = g_io_channel_write(gio, msg, strlen(msg), &bytes_written);
	if (gerror != G_IO_ERROR_NONE)
	{
		g_warning("Error sending message: %s : bytes written %d\n", msg,
				  (int)bytes_written);
	}
}

static void sendprocesslist(GIOChannel * gio)
{
	struct appwidgetinfo *appw_list;
	gchar *msg = NULL;
	msg = (gchar *) malloc((256 + 16) * sizeof(gchar));

	appw_list = appmanager_get_started_apps();
	while (appw_list != NULL)
	{
		if (strlen((const char *)appw_list->menu_elt->name) < 256)
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

static void handle_update_resolution(gchar * msg)
{
	gchar **contentssplit = NULL;
	gchar *name = NULL;
	int i = 0;
	int width, height;

	contentssplit = g_strsplit(msg, "::", 5);

	// Msg should contain 4 elements as according to protocol
	for (i = 0; i < 5; i++)
	{
		if (contentssplit[i] == NULL)
			return;
	}

	if (g_strcmp0(contentssplit[2], "general") == 0)
	{
		name = NULL;
	}
	else
	{
		name = contentssplit[2];
	}
	width = atoi(contentssplit[3]);
	height = atoi(contentssplit[4]);
	appmanager_update_resolution(name, width, height);

	g_strfreev(contentssplit);
}

static gboolean handleconnection(GIOChannel * gio, GIOCondition cond,
								 gpointer data)
{
	gsize len;
	gchar *msg;
	GError *gerror = NULL;
	GIOStatus status = G_IO_STATUS_NORMAL;
	int newsock;
	struct sockaddr_in cli_addr;
	socklen_t cli_len;
	GIOChannel *new_gio = NULL;
	int msg_id;

	cli_len = sizeof(cli_addr);
	newsock = accept(*(int *)data, (struct sockaddr *)&cli_addr, &cli_len);
	if (newsock < 0)
	{
		perror("Error accepting message:");
		// we return TRUE to keep watch active.
		return TRUE;
	}

	if (cond & G_IO_IN)
	{
		// TODO: fork to handle connections concurrently
		new_gio = g_io_channel_unix_new(newsock);
		g_io_channel_set_buffer_size(new_gio, 0);
		g_io_channel_set_encoding(new_gio, "UTF-8", NULL);
		g_io_channel_set_line_term(new_gio, NULL, -1);

		status = g_io_channel_read_line(new_gio, &msg, &len, NULL, &gerror);
		if (status == G_IO_STATUS_ERROR)
		{
			g_warning("Listener (handleconnection): %s\n", gerror->message);
		}
		else
		{
			msg_id = parsemessage(msg);
			switch (msg_id)
			{
			case SEND_PROCESS_LIST:
				sendprocesslist(new_gio);
				break;;
			case SEND_FONTSIZE:
				g_free(msg);
				msg = (gchar *) malloc(16 * sizeof(gchar));
				g_sprintf(msg, "fontsize::%d", gm_get_fontsize());
				writemsg(new_gio, msg);
				break;;
			case UPDATE_RES:
				handle_update_resolution(msg);
				break;;
			case SEND_CONFPATH:
				g_free(msg);
				msg =
					(gchar *) malloc((10 + strlen(confpath)) * sizeof(gchar));
				g_sprintf(msg, "confpath::%s", confpath);
				writemsg(new_gio, msg);
				break;;
			}
			g_free(msg);
		}
		listener_socket_close(new_gio);
	}
	// Always return TRUE to keep watch active.
	return TRUE;
}

gboolean listener_socket_open(GtkWidget * win)
{
	static int sock;
	int s;
	struct addrinfo hints;
	struct addrinfo *result = NULL;
	struct addrinfo *rp = NULL;
	const gchar *server = "localhost";
	const gchar *port = "2103";
	gboolean listener_started = FALSE;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = 0;
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	s = getaddrinfo(NULL, "2103", &hints, &result);
	if (s != 0)
	{
		g_warning("getaddrinfo: %s\n", gai_strerror(s));
	}
	else
	{
		for (rp = result; rp != NULL; rp = rp->ai_next)
		{
			sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

			if (sock == -1)
				continue;

			if (bind(sock, rp->ai_addr, rp->ai_addrlen) == 0)
			{
				break;			/* Success */
			}
			close(sock);
		}
		if (rp != NULL)			/* An address succeeded */
		{
			// Listen for TCP connections
			if (listen(sock, 5) >= 0)
			{
				listener_started = TRUE;
			}
		}
		freeaddrinfo(result);	/* No longer needed */

		if (listener_started == TRUE)
		{
			gio = g_io_channel_unix_new(sock);

			if (!g_io_add_watch
				(gio, G_IO_IN, handleconnection, (gpointer) & sock))
			{
				g_warning("Cannot add watch on GIOChannel!\n");
				listener_started = FALSE;
			}
		}
	}

	if (listener_started == TRUE)
	{
		g_message("Listening on port %s on %s\n", port, server);
		return TRUE;
	}
	else
	{
		close(sock);
		g_warning("Could not start listener");
		gm_show_confirmation_dialog
			("Could not start listener.\nShould I try again?",
			 "Restart listener", listener_socket_open, win, "Cancel", NULL,
			 NULL, win);
		return FALSE;
	}
}


gboolean listener_socket_close(GIOChannel * close_gio)
{
	GIOStatus status;
	GError *gerror = NULL;
	if (close_gio == NULL)
	{
		close_gio = gio;
	}
	if (close_gio == NULL)
	{
		return FALSE;
	}

	status = g_io_channel_shutdown(close_gio, TRUE, &gerror);
	if (status == G_IO_STATUS_ERROR)
	{
		g_warning("Listener (listener_socket_close): %s\n", gerror->message);
		return FALSE;
	}

	return TRUE;
}

void listener_socket_set_confpath(const gchar * path)
{
	confpath = path;
}
