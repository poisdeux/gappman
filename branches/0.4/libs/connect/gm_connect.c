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
#include "gm_connect.h"

static void parse_confpath_message(gchar **path, gchar *msg)
{
    gchar** contentssplit = NULL;
    int i = 0;
    contentssplit = g_strsplit(msg, "::", 0);
    while ( contentssplit[i] != NULL )
    {
        if ( g_strcmp0("confpath", contentssplit[i]) == 0)
        {
            *path = contentssplit[i+1];
        }
        i++;
    }
}


static void parse_fontsize_message(int *fontsize, gchar *msg)
{
    gchar** contentssplit = NULL;
    int i = 0;
    contentssplit = g_strsplit(msg, "::", 0);

    while ( contentssplit[i] != NULL )
    {
        if ( g_strcmp0("fontsize", contentssplit[i]) == 0)
        {
            *fontsize = atoi(contentssplit[i+1]);
        }
        i++;
    }
}

int gm_connect_to_gappman(int portno, const char* hostname, int *sockfd)
{
#ifdef NO_LISTENER
    return GM_NET_COMM_NOT_SUPPORTED;
#else
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

    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(*sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        g_warning("Error: could not connect.\n");
        return GM_COULD_NOT_CONNECT;
    }
    return GM_SUCCES;
#endif
}

static int create_gio_channel(int portno, const char* hostname, GIOChannel** gio, int *sockfd)
{
    int status;

    status = gm_connect_to_gappman(portno, hostname, sockfd);
    if (status != GM_SUCCES)
    {
        return GM_COULD_NOT_CONNECT;
    }
    *gio = g_io_channel_unix_new (*sockfd);
    g_io_channel_set_buffer_size (*gio, 0);
    g_io_channel_set_line_term (*gio, NULL, 2);
    g_io_channel_set_encoding (*gio, "UTF-8", NULL);

    return GM_SUCCES;
}

int gm_get_confpath_from_gappman(int portno, const char* hostname, gchar** path)
{
#ifdef NO_LISTENER
    return GM_NET_COMM_NOT_SUPPORTED;
#else
    gsize len;
    int socket;
    gchar *msg;
    int status;
    gsize bytes_written;
    GIOChannel* gio = NULL;
    GError *gerror = NULL;


    status = create_gio_channel(portno, hostname, &gio, &socket);
    if ( status != GM_SUCCES )
        return status;
    msg = g_strdup("::showconfpath::\n");
    status = g_io_channel_write_chars(gio, (const gchar*) msg, -1, &bytes_written, &gerror);
    if ( status == G_IO_STATUS_ERROR )
    {
        g_warning("gm_get_confpath_from_gappman: %s\n", gerror->message );
        return GM_COULD_NOT_SEND_MESSAGE;
    }

    msg = g_strdup("::showconfpath::\n");
    status = g_io_channel_flush( gio, &gerror);
    if ( status == G_IO_STATUS_ERROR )
    {
        g_warning("gm_get_confpath_from_gappman: %s\n", gerror->message );
    }

    g_free(msg);

    while ( g_io_channel_read_line( gio, &msg, &len, NULL,  &gerror) != G_IO_STATUS_EOF )
    {
		parse_confpath_message(path, msg);
    }

    g_free(msg);

    g_io_channel_unref(gio);
    //status = g_io_channel_shutdown( gio, TRUE, &gerror );
    if ( status == G_IO_STATUS_ERROR )
    {
        g_warning("gm_get_confpath_from_gappman: %s\n", gerror->message );
        return GM_COULD_NOT_DISCONNECT;
    }

    close(socket);

    return GM_SUCCES;
#endif
}


int gm_get_fontsize_from_gappman(int portno, const char* hostname, int *fontsize)
{
#ifdef NO_LISTENER
    return GM_NET_COMM_NOT_SUPPORTED;
#else
    gsize len;
    int socket;
    gchar *msg;
    int status;
    gsize bytes_written;
    GIOChannel* gio = NULL;
    GError *gerror = NULL;

    status = create_gio_channel(portno, hostname, &gio, &socket);
    if ( status != GM_SUCCES )
        return status;
    msg = g_strdup("::showfontsize::\n");
    status = g_io_channel_write_chars(gio, (const gchar*) msg, -1, &bytes_written, &gerror);
    if ( status == G_IO_STATUS_ERROR )
    {
        g_warning("gm_get_fontsize_from_gappman: %s\n", gerror->message );
        return GM_COULD_NOT_SEND_MESSAGE;
    }

    status = g_io_channel_flush( gio, &gerror);
    if ( status == G_IO_STATUS_ERROR )
    {
        g_warning("gm_get_fontsize_from_gappman: %s\n", gerror->message );
    }

    g_free(msg);

    while ( g_io_channel_read_line( gio, &msg, &len, NULL,  &gerror) != G_IO_STATUS_EOF )
    {
        parse_fontsize_message(fontsize, msg);
    }


    g_io_channel_unref(gio);
    //status = g_io_channel_shutdown( gio, TRUE, &gerror );
    if ( status == G_IO_STATUS_ERROR )
    {
        g_warning("gm_get_fontsize_from_gappman: %s\n", gerror->message );
        return GM_COULD_NOT_DISCONNECT;
    }

    close(socket);

    return GM_SUCCES;
#endif
}

int gm_send_and_receive_message(int portno, const char* hostname, gchar *msg, void (*callbackfunc)(gchar*))
{
#ifdef NO_LISTENER
    return GM_NET_COMM_NOT_SUPPORTED;
#else
    gsize len;
    int socket;
    int status;
    gsize bytes_written;
    GIOChannel* gio = NULL;
    GError *gerror = NULL;
	gchar* recv_msg;

    status = create_gio_channel(portno, hostname, &gio, &socket);
    if ( status != GM_SUCCES )
        return status;
    status = g_io_channel_write_chars(gio, (const gchar*) msg, -1, &bytes_written, &gerror);
    if ( status == G_IO_STATUS_ERROR )
    {
        g_warning("gm_send_and_receive_message: %s\n", gerror->message );
        return GM_COULD_NOT_SEND_MESSAGE;
    }

    status = g_io_channel_flush( gio, &gerror);
    if ( status == G_IO_STATUS_ERROR )
    {
        g_warning("gm_send_and_receive_message: %s\n", gerror->message );
    }

	if( callbackfunc != NULL )
    {
		while ( g_io_channel_read_line( gio, &recv_msg, &len, NULL,  &gerror) != G_IO_STATUS_EOF )
    	{
			g_warning("I should be calling the callback function right now. But my master has not yet implemented this feature!");
    	}
	}

    g_io_channel_unref(gio);
    status = g_io_channel_shutdown( gio, TRUE, &gerror );
    if ( status == G_IO_STATUS_ERROR )
    {
        g_warning("gm_send_and_receive_message: %s\n", gerror->message );
        return GM_COULD_NOT_DISCONNECT;
    }

    close(socket);

    return GM_SUCCES;
#endif
}
