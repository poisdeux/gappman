#include <gtk/gtk.h>

static gdouble linewidth = 10.0;
static double vert_bar_length;
static double hor_bar_length;
static GtkWidget *window;
static gint digit;

static gint bars_for_digit[10][7];

static void create_bars_for_digit()
{
	//digit 0
	bars_for_digit[0][0] = 1;
	bars_for_digit[0][1] = 1;
	bars_for_digit[0][2] = 1;
	bars_for_digit[0][3] = 0;
	bars_for_digit[0][4] = 1;
	bars_for_digit[0][5] = 1;
	bars_for_digit[0][6] = 1;
	//digit 1 
	bars_for_digit[1][0] = 0;
	bars_for_digit[1][1] = 0;
	bars_for_digit[1][2] = 1;
	bars_for_digit[1][3] = 0;
	bars_for_digit[1][4] = 0;
	bars_for_digit[1][5] = 1;
	bars_for_digit[1][6] = 0;
	//digit 2 
	bars_for_digit[2][0] = 1;
	bars_for_digit[2][1] = 0;
	bars_for_digit[2][2] = 1;
	bars_for_digit[2][3] = 1;
	bars_for_digit[2][4] = 1;
	bars_for_digit[2][5] = 0;
	bars_for_digit[2][6] = 1;
	//digit 3
	bars_for_digit[3][0] = 1;
	bars_for_digit[3][1] = 0;
	bars_for_digit[3][2] = 1;
	bars_for_digit[3][3] = 1;
	bars_for_digit[3][4] = 0;
	bars_for_digit[3][5] = 1;
	bars_for_digit[3][6] = 1;
	//digit 4 
	bars_for_digit[4][0] = 0;
	bars_for_digit[4][1] = 1;
	bars_for_digit[4][2] = 1;
	bars_for_digit[4][3] = 1;
	bars_for_digit[4][4] = 0;
	bars_for_digit[4][5] = 1;
	bars_for_digit[4][6] = 0;
	//digit 5 
	bars_for_digit[5][0] = 1;
	bars_for_digit[5][1] = 1;
	bars_for_digit[5][2] = 0;
	bars_for_digit[5][3] = 1;
	bars_for_digit[5][4] = 0;
	bars_for_digit[5][5] = 1;
	bars_for_digit[5][6] = 1;
	//digit 6
	bars_for_digit[6][0] = 1;
	bars_for_digit[6][1] = 1;
	bars_for_digit[6][2] = 0;
	bars_for_digit[6][3] = 1;
	bars_for_digit[6][4] = 1;
	bars_for_digit[6][5] = 1;
	bars_for_digit[6][6] = 1;
	//digit 7
	bars_for_digit[7][0] = 1;
	bars_for_digit[7][1] = 0;
	bars_for_digit[7][2] = 1;
	bars_for_digit[7][3] = 0;
	bars_for_digit[7][4] = 0;
	bars_for_digit[7][5] = 1;
	bars_for_digit[7][6] = 0;
	//digit 8
	bars_for_digit[8][0] = 1;
	bars_for_digit[8][1] = 1;
	bars_for_digit[8][2] = 1;
	bars_for_digit[8][3] = 1;
	bars_for_digit[8][4] = 1;
	bars_for_digit[8][5] = 1;
	bars_for_digit[8][6] = 1;
	//digit 9
	bars_for_digit[9][0] = 1;
	bars_for_digit[9][1] = 1;
	bars_for_digit[9][2] = 1;
	bars_for_digit[9][3] = 1;
	bars_for_digit[9][4] = 0;
	bars_for_digit[9][5] = 1;
	bars_for_digit[9][6] = 1;
}

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

static void draw_bar(cairo_t *cr, int bar, gdouble x, gdouble y)
{
/*
bar positions:

 0		
1 2
 3
4 5
 6 
*/


	switch(bar)
	{
		case 0:
			draw_horizontal_bar(cr, x, y, hor_bar_length);
			break;
		case 1:
			draw_vertical_bar(cr, x - linewidth , y + linewidth, vert_bar_length);
			break;
		case 2:
			draw_vertical_bar(cr, x + hor_bar_length , y + linewidth, vert_bar_length);
			break;
		case 3:
			draw_horizontal_bar(cr, x, y + linewidth + vert_bar_length, hor_bar_length);
			break;
		case 4:
			draw_vertical_bar(cr, x - linewidth, y + (2*linewidth) + vert_bar_length, vert_bar_length);
			break;
		case 5:
			draw_vertical_bar(cr, x + hor_bar_length, y + (2*linewidth) + vert_bar_length, vert_bar_length);
			break;
		case 6:
			g_warning("drawing bar 6");
			draw_horizontal_bar(cr, x, y + (2*vert_bar_length) + (2*linewidth), hor_bar_length);
			break;
		default:
			g_warning("Sorry sir, but I have no knowledge of bar number %d", bar);
			break;
	}
}

static draw_digit(cairo_t *cr, int digit, gdouble x_offset, gdouble y_offset)
{
	int i;

	g_debug("drawing digit %d", digit);
	for(i = 0; i < 7; i++)
	{
		g_debug("bars_for_digit[%d][%d] = %d\n", digit, i, bars_for_digit[digit][i]);
		if( bars_for_digit[digit][i] == 1 )
		{
			draw_bar(cr, i, x_offset, y_offset);
		}
	}  
}


static gboolean
on_expose_event(GtkWidget *widget,
    GdkEventExpose *event,
    gpointer data)
{
	cairo_t *cr;
	double x, y;
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

	draw_digit(cr, digit, 10, 10);

	cairo_stroke (cr);

	cairo_destroy(cr);
	return FALSE;
}

static gboolean update_time(gpointer data)
{
	GtkWidget *widget;
	GdkRegion *region;

	g_warning("update_time");
	widget = GTK_WIDGET(data);	
	digit = random()%10;

	if (!widget->window)
		return FALSE;
	
  region = gdk_drawable_get_clip_region (widget->window);
  gdk_window_invalidate_region (widget->window, region, TRUE);
  gdk_window_process_updates (widget->window, TRUE);

  gdk_region_destroy (region);

	return TRUE;
}

int main(int argc, char** argv)
{
	
	gtk_init(&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);
	
	g_signal_connect(window, "expose-event",
      G_CALLBACK(on_expose_event), NULL);
  g_signal_connect(window, "destroy",
      G_CALLBACK(gtk_main_quit), NULL);

	create_bars_for_digit();

	gtk_widget_set_app_paintable(window, TRUE);

	g_signal_connect(G_OBJECT(window), "expose-event", G_CALLBACK(on_expose_event), NULL);
	
	g_timeout_add(1000, update_time, window);
	gtk_widget_show_all(window);

	gtk_main();
}
