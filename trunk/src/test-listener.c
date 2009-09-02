#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

int main(int argc, char *argv[])
{
	gsize len;
  gchar *msg;
  int status, sockfd, portno, n, sourceid;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  int bytes_written;
	GIOChannel* gio = NULL;
  GError *gerror = NULL;
  gchar buffer[256];
	gchar line[] = { ';', '\n'};

  if (argc < 3)
  {
    fprintf(stderr,"usage %s hostname port", argv[0]);
    exit(0);
  }

  portno = atoi(argv[2]);
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
    fprintf(stderr, "Error: could not open socket.\n");
  }

  server = gethostbyname(argv[1]);
  if (server == NULL)
  {
    fprintf(stderr,"Error: could not find host %s", argv[1]);
    exit(0);
  }

  memset((char *) &serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  memcpy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
  serv_addr.sin_port = htons(portno);

  if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
  {
    fprintf(stderr, "Error: could not connect.\n");
		exit(0);
  }

  printf("Please enter the message: ");
  memset(buffer, 0, 256);
  fgets(buffer,255,stdin);

	gio = g_io_channel_unix_new (sockfd);
  g_io_channel_set_buffer_size (gio, 0);
  g_io_channel_set_line_term (gio, line, 2);
  g_io_channel_set_encoding (gio, "UTF-8", NULL);

	status = g_io_channel_write_chars(gio, (const gchar*) buffer, -1, &bytes_written, &gerror);
  if( status == G_IO_STATUS_ERROR )
  {
    g_error("test-listener: %s\n", gerror->message );
  }

	status = g_io_channel_flush( gio, &gerror);
	if( status == G_IO_STATUS_ERROR )
  {
    g_error("test-listener: %s\n", gerror->message );
  }

	if( status == G_IO_STATUS_NORMAL )
  {
    fprintf(stdout, "MESSAGE SENT: %s\n", buffer);
	}

	while( g_io_channel_read_line( gio, &msg, &len, NULL,  &gerror) != G_IO_STATUS_EOF )
	{
		fprintf(stdout, "MESSAGE RECEIVED: %s\n", msg);
	}

	status = g_io_channel_shutdown( gio, TRUE, &gerror);
  if ( status == G_IO_STATUS_ERROR )
	{ 
     g_error ("test-listenerr (handlemessage): %s\n", gerror->message);
	}

  
  return 0;

}
