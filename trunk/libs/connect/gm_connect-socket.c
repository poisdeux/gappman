/**
 * \file gm_connect.c
 *
 *
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <sys/socket.h>
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <glib.h>
#include <string.h>
#include <gm_generic.h>
#include "gm_connect-generic.h"
#include "gm_connect-socket.h"

static gchar *parse_message(gchar * msg, gchar *keyword)
{
  gchar **contentssplit = NULL;
  int i = 0;
  contentssplit = g_strsplit(msg, "::", 0);
  while (contentssplit[i] != NULL)
  {
    if (g_strcmp0(keyword, contentssplit[i]) == 0)
    {
      return contentssplit[i + 1];
    }
    i++;
  }
}

static int send_and_receive_message(GIOChannel *gio, gchar **rcv_msg, gchar *send_msg)
{
  gsize len;
	gchar *msg = NULL;
	int status;
	gsize bytes_written;
	GError *gerror = NULL;

	status =
		g_io_channel_write_chars(gio, (const gchar *)send_msg, -1, &bytes_written,
								 &gerror);
	if (status == G_IO_STATUS_ERROR)
	{
		g_warning("gm_socket: %s\n",
				  gerror->message);
		return GM_COULD_NOT_SEND_MESSAGE;
	}

	status = g_io_channel_flush(gio, &gerror);
	if (status == G_IO_STATUS_ERROR)
	{
		g_warning("gm_socket: %s\n",
				  gerror->message);
	}

	status = g_io_channel_read_line(gio, &msg, NULL, NULL, &gerror);
	if (status == G_IO_STATUS_ERROR)
	{
		g_warning("gm_socket: %s\n",
          gerror->message);
		rcv_msg = NULL;
		return GM_COULD_NOT_RECEIVE_MESSAGE;
	}

	*rcv_msg = msg;
	
	return GM_SUCCES;
}

/**
* \brief starts a new connection to the application manager and creates a GIO channel
* \param portno portnumber the application manager is listening on
* \param hostname hostname of the machine where the application manager is running
* \param gio will hold the newly created GIO channel for the connection
* \param sockfd will hold the socket filedescriptor for this connection to close the socket later on
*/
static int create_gio_channel(int portno, const char *hostname,
							  GIOChannel ** gio)
{
	int status;
	int sockfd;

	status = gm_socket_connect_to_gappman(portno, hostname, &sockfd);
	if (status != GM_SUCCES)
	{
		return GM_COULD_NOT_CONNECT;
	}
	*gio = g_io_channel_unix_new(sockfd);
	g_io_channel_set_buffer_size(*gio, 0);
	g_io_channel_set_line_term(*gio, NULL, 2);
	g_io_channel_set_encoding(*gio, "UTF-8", NULL);

	return GM_SUCCES;
}

static void parseProceslistMessage(struct proceslist **procs, gchar * msg)
{
	gchar **contentssplit = NULL;
	int i = 0;
	int state = 0;
	contentssplit = g_strsplit(msg, "::", 0);

	while (contentssplit[i] != NULL)
	{
		if (g_strcmp0("name", contentssplit[i]) == 0)
		{
			*procs = createnewproceslist(*procs);
			(*procs)->name = contentssplit[i + 1];
			state = 1;
		}
		else if (g_strcmp0("pid", contentssplit[i]) == 0)
		{
			if (state == 1)
			{
				(*procs)->pid = atoi(contentssplit[i + 1]);
				state = 0;
			}
		}
		i++;
	}
}

int gm_socket_connect_to_gappman(int portno, const char *hostname, int *sockfd)
{
	struct sockaddr_in serv_addr;
	struct hostent *server;

	*sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (*sockfd < 0)
	{
		g_warning("could not open socket.\n");
		return GM_COULD_NOT_CONNECT;
	}
	server = gethostbyname(hostname);
	if (server == NULL)
	{
		g_warning("Error: could not find host %s", hostname);
		return GM_COULD_NOT_RESOLVE_HOSTNAME;
	}

	memset((char *)&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	memcpy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,
		   server->h_length);
	serv_addr.sin_port = htons(portno);

	if (connect(*sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		g_warning("Error: could not connect.\n");
		return GM_COULD_NOT_CONNECT;
	}

	return GM_SUCCES;
}

int gm_socket_get_started_procs_from_gappman(int portno, const char *hostname,
											 struct proceslist **startedprocs)
{
	gsize len;
	gchar *msg;
	int status;
	gsize bytes_written;
	GIOChannel *gio = NULL;
	GError *gerror = NULL;

	status = create_gio_channel(portno, hostname, &gio);
	if (status != GM_SUCCES)
		return status;

	msg = g_strdup("::listprocesses::\n");
	status =
		g_io_channel_write_chars(gio, (const gchar *)msg, -1, &bytes_written,
								 &gerror);
	if (status == G_IO_STATUS_ERROR)
	{
		g_warning("%s\n", gerror->message);
		return GM_COULD_NOT_SEND_MESSAGE;
	}

	g_io_channel_flush(gio, &gerror);

	g_free(msg);

	while (g_io_channel_read_line(gio, &msg, &len, NULL, &gerror) !=
		   G_IO_STATUS_EOF)
	{
		parseProceslistMessage(startedprocs, msg);
	}

	status = g_io_channel_shutdown(gio, TRUE, &gerror);
	if (status == G_IO_STATUS_ERROR)
	{
		g_warning("getStartedProcsFromGappman: %s\n", gerror->message);
		return GM_COULD_NOT_DISCONNECT;
	}

	return GM_SUCCES;
}

int gm_socket_get_confpath_from_gappman(int portno, const char *hostname,
										gchar ** path)
{
	int status;
	GError *gerror = NULL;
  gchar *recv_msg;
	GIOChannel *gio = NULL;
	
	status = create_gio_channel(portno, hostname, &gio);
	if (status != GM_SUCCES)
		return status;

	status = send_and_receive_message(gio, &recv_msg, "::showconfpath::\n");
	if( status != GM_SUCCES )
	{
		return status;
	}

	*path = parse_message(recv_msg, "confpath");

	status = g_io_channel_shutdown(gio, TRUE, &gerror);
	if (status == G_IO_STATUS_ERROR)
	{
		g_warning("gm_socket_get_confpath_from_gappman: %s\n",
				  gerror->message);
		return GM_COULD_NOT_DISCONNECT;
	}

	return GM_SUCCES;
}


int gm_socket_get_fontsize_from_gappman(int portno, const char *hostname,
										int *fontsize)
{
	int status;
	GError *gerror = NULL;
  gchar *recv_msg;
	GIOChannel *gio = NULL;

	status = create_gio_channel(portno, hostname, &gio);
	if (status != GM_SUCCES)
		return status;

	status = send_and_receive_message(gio, &recv_msg, "::showfontsize::\n");
	if( status != GM_SUCCES )
	{
		return status;
	}

	*fontsize = atoi(parse_message(recv_msg, "fontsize"));

	status = g_io_channel_shutdown(gio, TRUE, &gerror);
	if (status == G_IO_STATUS_ERROR)
	{
		g_warning("gm_socket_get_fontsize_from_gappman: %s\n",
				  gerror->message);
		return GM_COULD_NOT_DISCONNECT;
	}


	return GM_SUCCES;
}

int gm_socket_send_and_receive_message(int portno, const char *hostname,
									   gchar * msg,
									   void (*callbackfunc) (gchar *))
{
	gsize len;
	int status;
	gsize bytes_written;
	GIOChannel *gio = NULL;
	GError *gerror = NULL;
	gchar *recv_msg;

	status = create_gio_channel(portno, hostname, &gio);
	if (status != GM_SUCCES)
		return status;
	status =
		g_io_channel_write_chars(gio, (const gchar *)msg, -1, &bytes_written,
								 &gerror);
	if (status == G_IO_STATUS_ERROR)
	{
		g_warning("gm_socket_send_and_receive_message: %s\n", gerror->message);
		return GM_COULD_NOT_SEND_MESSAGE;
	}

	status = g_io_channel_flush(gio, &gerror);
	if (status == G_IO_STATUS_ERROR)
	{
		g_warning("gm_socket_send_and_receive_message: %s\n", gerror->message);
	}

	if (callbackfunc != NULL)
	{
		while (g_io_channel_read_line(gio, &recv_msg, &len, NULL, &gerror) !=
			   G_IO_STATUS_EOF)
		{
			g_warning
				("I should be calling the callback function right now. But my master has not yet implemented this feature!");
		}
	}

	status = g_io_channel_shutdown(gio, TRUE, &gerror);
	if (status == G_IO_STATUS_ERROR)
	{
		g_warning("gm_socket_send_and_receive_message: %s\n", gerror->message);
		return GM_COULD_NOT_DISCONNECT;
	}

	return GM_SUCCES;
}

int gm_socket_set_default_resolution_for_program(int portno,
												 const char *hostname,
												 gchar * name, int width,
												 int height)
{
	int status;
	gsize bytes_written;
	GIOChannel *gio = NULL;
	GError *gerror = NULL;
	gchar *msg;

	status = create_gio_channel(portno, hostname, &gio);
	if (status != GM_SUCCES)
		return status;
	msg = g_strdup_printf("::updateres::%s::%d::%d::", name, width, height);
	status =
		g_io_channel_write_chars(gio, (const gchar *)msg, -1, &bytes_written,
								 &gerror);
	if (status == G_IO_STATUS_ERROR)
	{
		g_warning("gm_socket_set_default_resolution_for_program: %s\n",
				  gerror->message);
		return GM_COULD_NOT_SEND_MESSAGE;
	}

	status = g_io_channel_flush(gio, &gerror);
	if (status == G_IO_STATUS_ERROR)
	{
		g_warning("gm_socket_set_default_resolution_for_program: %s\n",
				  gerror->message);
	}

	g_free(msg);

	status = g_io_channel_shutdown(gio, TRUE, &gerror);
	if (status == G_IO_STATUS_ERROR)
	{
		g_warning("gm_socket_set_default_resolution_for_program: %s\n",
				  gerror->message);
		return GM_COULD_NOT_DISCONNECT;
	}

	return GM_SUCCES;
}

#if defined(DEBUG)
int gm_socket_get_window_geometry_from_gappman(int portno, int *width, int *height)
{
	int status;
	GError *gerror = NULL;
  gchar *recv_msg;
  gchar *geom_msg;
	GIOChannel *gio = NULL;

	status = create_gio_channel(portno, hostname, &gio);
	if (status != GM_SUCCES)
		return status;

	status = send_and_receive_message(gio, &recv_msg, "::showwindowgeometry::\n");
	if( status != GM_SUCCES )
	{
		return status;
	}

	geom_msg = parse_message(recv_msg, "windowgeometry");

	g_debug("geom_msg: %s", geom_msg);

	status = g_io_channel_shutdown(gio, TRUE, &gerror);
	if (status == G_IO_STATUS_ERROR)
	{
		g_warning("gm_socket_get_fontsize_from_gappman: %s\n",
				  gerror->message);
		return GM_COULD_NOT_DISCONNECT;
	}

	return GM_SUCCES;
}
#endif

