#include <gtk/gtk.h>

#define LINEWIDTH 10

static gboolean draw_horizontal_bar(cairo_t *cr, int x, int y, int length)
{

	double halflinewidth = LINEWIDTH/2;

	//draw left triangle
	cairo_move_to(cr, x, y);
	cairo_rel_line_to(cr, -halflinewidth, halflinewidth);
	cairo_rel_line_to(cr, halflinewidth, halflinewidth);
	cairo_close_path(cr);

	//draw vertical line
	cairo_rectangle(cr, x, y, length, LINEWIDTH);

	//draw right triangle
	cairo_move_to(cr, x+length, y);
	cairo_rel_line_to(cr, halflinewidth, halflinewidth);
	cairo_rel_line_to(cr, -halflinewidth, halflinewidth);
	cairo_close_path(cr);

	cairo_set_line_join (cr, CAIRO_LINE_JOIN_ROUND);
	cairo_fill(cr);
}


static gboolean draw_vertical_bar(cairo_t *cr, double x, double y, double length)
{

	double halflinewidth = LINEWIDTH/2;

	//draw top triangle
	cairo_move_to(cr, x, y);
	cairo_rel_line_to(cr, halflinewidth, -halflinewidth);
	cairo_rel_line_to(cr, halflinewidth, halflinewidth);
	cairo_close_path(cr);

	//draw vertical line
	cairo_rectangle(cr, x, y, LINEWIDTH, length);

	//draw bottom triangle
	cairo_move_to(cr, x, y+length);
	cairo_rel_line_to(cr, halflinewidth, halflinewidth);
	cairo_rel_line_to(cr, halflinewidth, -halflinewidth);
	cairo_close_path(cr);

	cairo_set_line_join (cr, CAIRO_LINE_JOIN_ROUND);
	cairo_fill(cr);
}

static gboolean
on_expose_event(GtkWidget *widget,
    GdkEventExpose *event,
    gpointer data)
{
	cairo_t *cr;
	double x, y;
	double length;

	cr = gdk_cairo_create (widget->window);

	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	cairo_set_line_width(cr, 0);

	// Number 5
	x = 50.0;
	y = 50.0;
	length = 20.0;

/*

	bar positions:

	 1
  2 3
   4
  5 6
   7
*/

	//pos 1
	draw_horizontal_bar(cr, x, y, length);
	//pos 2
	draw_vertical_bar(cr, x - LINEWIDTH , y + LINEWIDTH, length);
	//pos 3
	draw_vertical_bar(cr, x + length , y + LINEWIDTH, length);
	//pos 4 
	draw_horizontal_bar(cr, x, y + LINEWIDTH + length, length);
	//pos 5 
	draw_vertical_bar(cr, x - LINEWIDTH, y + (2*LINEWIDTH) + length, length);
	//pos 6
	draw_vertical_bar(cr, x + length, y + (2*LINEWIDTH) + length, length);
	//pos 7
	draw_horizontal_bar(cr, x, y + (2*length) + (2*LINEWIDTH), 20);

	cairo_stroke (cr);

	cairo_destroy(cr);
	return FALSE;
}

int main(int argc, char** argv)
{
	GtkWidget *window;
	
	gtk_init(&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	g_signal_connect(window, "expose-event",
      G_CALLBACK(on_expose_event), NULL);
  g_signal_connect(window, "destroy",
      G_CALLBACK(gtk_main_quit), NULL);

	gtk_widget_set_app_paintable(window, TRUE);

	g_signal_connect(G_OBJECT(window), "expose-event", G_CALLBACK(on_expose_event), NULL);
	
	gtk_widget_show_all(window);

	gtk_main();
}
