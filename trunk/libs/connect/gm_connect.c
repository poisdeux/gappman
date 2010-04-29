/***
 * \file gm_connect.c
 *
 * 
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 */


#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <gm_generic.h>
#include "gm_connect.h"


static void parse_fontsize_message(int *fontsize, gchar *msg)
{
	gchar** contentssplit = NULL;
	int i = 0;
	contentssplit = g_strsplit(msg, "::", 0);
	
	while( contentssplit[i] != NULL )
	{
		if( g_strcmp0("fontsize", contentssplit[i]) == 0)
		{
			*fontsize = atoi(contentssplit[i+1]);
		}
		i++; 
	}
}

int gm_connect_to_gappman(int portno, const char* hostname, int *sockfd)
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
}

static int create_gio_channel(int portno, const char* hostname, GIOChannel** gio)
{
	int sockfd;
	int status;

  status = gm_connect_to_gappman(portno, hostname, &sockfd);
	if(status != GM_SUCCES)
	{	
		return GM_COULD_NOT_CONNECT;
	}
  *gio = g_io_channel_unix_new (sockfd);
  g_io_channel_set_buffer_size (*gio, 0);
  g_io_channel_set_line_term (*gio, NULL, 2);
  g_io_channel_set_encoding (*gio, "UTF-8", NULL);

	return GM_SUCCES;
}


int gm_get_fontsize_from_gappman(int portno, const char* hostname, int *fontsize)
{
  gsize len;
  gchar *msg;
  int status, n, sourceid;
  int bytes_written;
  GIOChannel* gio = NULL;
  GError *gerror = NULL;

	status = create_gio_channel(portno, hostname, &gio);
	if ( status != GM_SUCCES )
		return status; 

	msg = g_strdup("::showfontsize::\n");
	status = g_io_channel_write_chars(gio, (const gchar*) msg, -1, &bytes_written, &gerror);
  if( status == G_IO_STATUS_ERROR )
  {
    g_warning("gm_get_fontsize_from_gappman: %s\n", gerror->message );
		return GM_COULD_NOT_SEND_MESSAGE; 
  }

  status = g_io_channel_flush( gio, &gerror);
  if( status == G_IO_STATUS_ERROR )
  {
    g_warning("gm_get_fontsize_from_gappman: %s\n", gerror->message );
  }

	g_free(msg);

  while( g_io_channel_read_line( gio, &msg, &len, NULL,  &gerror) != G_IO_STATUS_EOF )
  {
		parse_fontsize_message(fontsize, msg);
  }

  status = g_io_channel_shutdown( gio, TRUE, &gerror);
  if ( status == G_IO_STATUS_ERROR )
  {
    g_warning("gm_get_fontsize_from_gappman: %s\n", gerror->message );
		return GM_COULD_NOT_DISCONNECT;
  }

	return GM_SUCCES;
}

int gm_send_and_receive_message(int portno, const char* hostname, gchar *msg, void (*callbackfunc)(gchar*))
{
  gsize len;
  gchar *receive_msg;
  int status, sockfd, n, sourceid;
  int bytes_written;
  GIOChannel* gio = NULL;
  GError *gerror = NULL;

  status = gm_connect_to_gappman(portno, hostname, &sockfd);
  if(status != GM_SUCCES)
  {
    return status;
  }

  gio = g_io_channel_unix_new (sockfd);
  g_io_channel_set_buffer_size (gio, 0);
  g_io_channel_set_line_term (gio, NULL, 2);
  g_io_channel_set_encoding (gio, "UTF-8", NULL);

  status = g_io_channel_write_chars(gio, (const gchar*) msg, -1, &bytes_written, &gerror);
  if( status == G_IO_STATUS_ERROR )
  {
    g_warning("%s\n", gerror->message );
                return GM_COULD_NOT_SEND_MESSAGE;
  }

  status = g_io_channel_flush( gio, &gerror);
  if( status == G_IO_STATUS_ERROR )
  {
    g_warning("%s\n", gerror->message );
  }

  if(callbackfunc != NULL)
  {
  	while( g_io_channel_read_line( gio, &receive_msg, &len, NULL,  &gerror) != G_IO_STATUS_EOF )
  	{
    		callbackfunc(receive_msg);
  	}
  }

  status = g_io_channel_shutdown( gio, TRUE, &gerror);
  if ( status == G_IO_STATUS_ERROR )
  {
        g_warning("handlemessage: %s\n", gerror->message);
        return GM_COULD_NOT_DISCONNECT;
  }

  return GM_SUCCES;
}

