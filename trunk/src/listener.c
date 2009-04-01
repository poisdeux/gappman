gboolean gappman_connect (const gchar *server, gint port)
{

	GIOChannel* gio;
	
	int sock;
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
		if (connect (sock, (struct sockaddr *) &addr, sizeof (addr)) == -1)
			return FALSE;


	}
	else
		return FALSE;
	gio = g_io_channel_unix_new (sock);
	g_io_channel_set_buffer_size (gio, 0);
	g_io_channel_set_line_term (gio, line, 2);
	g_io_channel_set_encoding (gio, "UTF-8", NULL);
	
	gift_io.source = g_io_create_watch (gio, G_IO_IN | G_IO_PRI);
	g_source_set_priority (gio.source, G_PRIORITY_LOW);
	g_source_set_callback (gio.source, (GSourceFunc) gift_receive,
			       NULL, NULL);

	g_source_attach (gio.source, NULL);
	return TRUE;
}


