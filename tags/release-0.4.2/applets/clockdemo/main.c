/**
 * main.c
 *
 * Test EggClockFace in a GtkWindow
 *
 * (c) 2005, Davyd Madeley
 *
 * Authors:
 *   Davyd Madeley <davyd@madeley.id.au>
 */

#include <gtk/gtk.h>

#include "clock.h"

int main(int argc, char **argv)
{
	GtkWidget *window;
	GtkWidget *clock;

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	clock = egg_clock_face_new();
	gtk_container_add(GTK_CONTAINER(window), clock);

	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	gtk_widget_show_all(window);

	gtk_main();
}
