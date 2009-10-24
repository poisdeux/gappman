#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include "connect.h"


void freeproceslist(struct proceslist* procslist)
{
	struct proceslist* tmp;
	while(procslist != NULL)
	{
		tmp = procslist->prev;
		free(procslist);
		procslist = tmp;
	}
}

/**
* \brief creates a new proceslist struct and links it to the list procs
* \param procs list of proceslist structs. If this is the first this should be NULL
* \return pointer to the last proceslist struct
*/
static struct proceslist* createnewproceslist(struct proceslist* procs)
{
	struct proceslist* newproc;
	newproc = (struct proceslist*) g_try_malloc0(sizeof(struct proceslist));
	if( newproc != NULL)
	{
		newproc->name = "";
		newproc->pid = -1;
		newproc->prev = procs;
	}
	return newproc;
}
static void parseFontsizeMessage(int *fontsize, gchar *msg)
{
	gchar** contentssplit = NULL;
	int i = 0;
	contentssplit = g_strsplit(msg, "::", 0);
	
	while( contentssplit[i] != NULL )
	{
		if( g_strcmp0("fontsize", contentssplit[i]) == 0)
		{
			printf("DEBUG: (parseFontsizeMessage) processing fontsize\n");
			fflush(stdout);
			*fontsize = contentssplit[i+1];
		}
		i++; 
	}
}

static void parseProceslistMessage(struct proceslist** procs, gchar *msg)
{
	gchar** contentssplit = NULL;
	int i = 0;
	contentssplit = g_strsplit(msg, "::", 0);
	
	while( contentssplit[i] != NULL )
	{
		if( g_strcmp0("name", contentssplit[i]) == 0)
		{
			printf("DEBUG: (parseMessage) processing name\n");
			fflush(stdout);
			*procs = createnewproceslist(*procs);
			(*procs)->name = contentssplit[i+1];
		}
		else if ( g_strcmp0("pid", contentssplit[i]) == 0)
		{
			printf("DEBUG: (parseMessage) processing pid\n");
			fflush(stdout);
			(*procs)->pid = atoi(contentssplit[i+1]);
		}
		i++; 
	}
}

int getInfoFromGappman(int portno, const char* hostname, struct proceslist **startedprocs, int *fontsize)
{
  gsize len;
  gchar *msg;
  int status, sockfd, n, sourceid;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  int bytes_written;
  GIOChannel* gio = NULL;
  GError *gerror = NULL;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
    fprintf(stderr, "Error: could not open socket.\n");
  }

  server = gethostbyname(hostname);
  if (server == NULL)
  {
    g_warning("Error: could not find host %s", hostname);
		return 1;
  }

  memset((char *) &serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  memcpy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
  serv_addr.sin_port = htons(portno);

  if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
  {
    g_warning("Error: could not connect.\n");
    return 2;
  }

  gio = g_io_channel_unix_new (sockfd);
  g_io_channel_set_buffer_size (gio, 0);
  g_io_channel_set_line_term (gio, NULL, 2);
  g_io_channel_set_encoding (gio, "UTF-8", NULL);

	msg = g_strdup("listprocesses\n");
	status = g_io_channel_write_chars(gio, (const gchar*) msg, -1, &bytes_written, &gerror);
  if( status == G_IO_STATUS_ERROR )
  {
    g_warning("test-listener: %s\n", gerror->message );
		return 3;
  }

  status = g_io_channel_flush( gio, &gerror);
  if( status == G_IO_STATUS_ERROR )
  {
    g_error("test-listener: %s\n", gerror->message );
  }

  if( status == G_IO_STATUS_NORMAL )
  {
    g_debug("MESSAGE SENT: %s\n", msg);
  }

	g_free(msg);

  while( g_io_channel_read_line( gio, &msg, &len, NULL,  &gerror) != G_IO_STATUS_EOF )
  {
    g_debug(stdout, "MESSAGE RECEIVED: %s\n", msg);
		parseProceslistMessage(startedprocs, msg);
  }
	
	msg = g_strdup("showfontsize\n");
	status = g_io_channel_write_chars(gio, (const gchar*) msg, -1, &bytes_written, &gerror);
  if( status == G_IO_STATUS_ERROR )
  {
    g_warning("test-listener: %s\n", gerror->message );
		return 3;
  }

  status = g_io_channel_flush( gio, &gerror);
  if( status == G_IO_STATUS_ERROR )
  {
    g_error("test-listener: %s\n", gerror->message );
  }

  if( status == G_IO_STATUS_NORMAL )
  {
    g_debug("MESSAGE SENT: %s\n", msg);
  }

	g_free(msg);

  while( g_io_channel_read_line( gio, &msg, &len, NULL,  &gerror) != G_IO_STATUS_EOF )
  {
    g_debug(stdout, "MESSAGE RECEIVED: %s\n", msg);
		parseFontsizeMessage(fontsize, msg);
  }

  status = g_io_channel_shutdown( gio, TRUE, &gerror);
  if ( status == G_IO_STATUS_ERROR )
  {
     g_debug("test-listener (handlemessage): %s\n", gerror->message);
			return 4;
  }

	printf("DEBUG: connect finished\n");
	fflush(stdout);

	return 0;
}
