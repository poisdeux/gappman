#include <gtk/gtk.h>

static gdouble linewidth = 10.0;

static gboolean draw_horizontal_bar(cairo_t *cr, int x, int y, int length)
{

	double halflinewidth = linewidth/2;

	//draw left triangle
	cairo_move_to(cr, x, y);
	cairo_rel_line_to(cr, -halflinewidth, halflinewidth);
	cairo_rel_line_to(cr, halflinewidth, halflinewidth);
	cairo_close_path(cr);

	//draw vertical line
	cairo_rectangle(cr, x, y, length, linewidth);

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

	double halflinewidth = linewidth/2;

	//draw top triangle
	cairo_move_to(cr, x, y);
	cairo_rel_line_to(cr, halflinewidth, -halflinewidth);
	cairo_rel_line_to(cr, halflinewidth, halflinewidth);
	cairo_close_path(cr);

	//draw vertical line
	cairo_rectangle(cr, x, y, linewidth, length);

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
	double vert_bar_length;
	double hor_bar_length;
	gint w_width, w_height;

	cr = gdk_cairo_create (widget->window);

	gtk_window_get_size(GTK_WINDOW(widget), &w_width, &w_height);

	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	cairo_set_line_width(cr, 0);

	x = 20.0;
	y = 10.0;
	vert_bar_length = (w_height/2) - 20.0;
	hor_bar_length =  (w_width/5) - 5;

	linewidth = hor_bar_length/5;
/*

	bar positions:

	 1
  2 3
   4
  5 6
   7
*/

	//pos 1
	draw_horizontal_bar(cr, x, y, hor_bar_length);
	//pos 2
	draw_vertical_bar(cr, x - linewidth , y + linewidth, vert_bar_length);
	//pos 3
	draw_vertical_bar(cr, x + hor_bar_length , y + linewidth, vert_bar_length);
	//pos 4 
	draw_horizontal_bar(cr, x, y + linewidth + vert_bar_length, hor_bar_length);
	//pos 5 
	draw_vertical_bar(cr, x - linewidth, y + (2*linewidth) + vert_bar_length, vert_bar_length);
	//pos 6
	draw_vertical_bar(cr, x + hor_bar_length, y + (2*linewidth) + vert_bar_length, vert_bar_length);
	//pos 7
	draw_horizontal_bar(cr, x, y + (2*vert_bar_length) + (2*linewidth), hor_bar_length);

	cairo_stroke (cr);

	cairo_destroy(cr);
	return FALSE;
}

int main(int argc, char** argv)
{
	GtkWidget *window;
	
	gtk_init(&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);
	
	g_signal_connect(window, "expose-event",
      G_CALLBACK(on_expose_event), NULL);
  g_signal_connect(window, "destroy",
      G_CALLBACK(gtk_main_quit), NULL);

	gtk_widget_set_app_paintable(window, TRUE);

	g_signal_connect(G_OBJECT(window), "expose-event", G_CALLBACK(on_expose_event), NULL);
	
	gtk_widget_show_all(window);

	gtk_main();
}
