#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <gm_connect.h>
#include <gm_generic.h>
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

static void parseProceslistMessage(struct proceslist** procs, gchar *msg)
{
	gchar** contentssplit = NULL;
	int i = 0;
	int state = 0;
	contentssplit = g_strsplit(msg, "::", 0);
	
	while( contentssplit[i] != NULL )
	{
		if( g_strcmp0("name", contentssplit[i]) == 0)
		{
			*procs = createnewproceslist(*procs);
			(*procs)->name = contentssplit[i+1];
			state = 1;
		}
		else if ( g_strcmp0("pid", contentssplit[i]) == 0)
		{	
			if ( state == 1 )
			{
				(*procs)->pid = atoi(contentssplit[i+1]);
				state = 0;
			}
		}
		i++; 
	}
}

int getStartedProcsFromGappman(int portno, const char* hostname, struct proceslist **startedprocs)
{
  gsize len;
  gchar *msg;
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

	msg = g_strdup("::listprocesses::\n");
	status = g_io_channel_write_chars(gio, (const gchar*) msg, -1, &bytes_written, &gerror);
  if( status == G_IO_STATUS_ERROR )
  {
    g_warning("%s\n", gerror->message );
		return 3;
  }

  status = g_io_channel_flush( gio, &gerror);
  if( status == G_IO_STATUS_ERROR )
  {
    g_warning("%s\n", gerror->message );
  }

	g_free(msg);

  while( g_io_channel_read_line( gio, &msg, &len, NULL,  &gerror) != G_IO_STATUS_EOF )
  {
	g_debug("TEST: message received: %s\n", msg);
		parseProceslistMessage(startedprocs, msg);
  }
	
  status = g_io_channel_shutdown( gio, TRUE, &gerror);
  if ( status == G_IO_STATUS_ERROR )
  {
     	g_warning("handlemessage: %s\n", gerror->message);
			return 4;
  }

	fflush(stdout);

	return 0;
}
