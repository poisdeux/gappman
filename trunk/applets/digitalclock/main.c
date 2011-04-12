/**
 * \file main.c
 * \brief Creates a window showing the time in digital letters as some LED displays show
 *
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 *
 * \todo see if drawing of digits can be done only once a minute, instead of each second
 * \todo fix horizontal measures to match diagram
 * \todo add configuration option for color (through gtkrc?)
 */

#include <gtk/gtk.h>

static gdouble linewidth = 10.0;
static gdouble vert_bar_length;
static gdouble hor_bar_length;
static gdouble column_width;
static struct tm time_tm;
static gint bars_for_digit[10][7];
static gdouble x_1_4_offset, x_2_5_offset,y_1_2_offset, y_4_5_offset, y_3_offset, y_6_offset;
static gdouble x_delta;

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
			draw_vertical_bar(cr, x + x_1_4_offset , y + y_1_2_offset, vert_bar_length);
			break;
		case 2:
			draw_vertical_bar(cr, x + x_2_5_offset, y + y_1_2_offset, vert_bar_length);
			break;
		case 3:
			draw_horizontal_bar(cr, x, y + y_3_offset, hor_bar_length);
			break;
		case 4:
			draw_vertical_bar(cr, x + x_1_4_offset, y + y_4_5_offset, vert_bar_length);
			break;
		case 5:
			draw_vertical_bar(cr, x + x_2_5_offset, y + y_4_5_offset, vert_bar_length);
			break;
		case 6:
			draw_horizontal_bar(cr, x, y + y_6_offset, hor_bar_length);
			break;
		default:
			g_warning("Sorry sir, but I have no knowledge of bar number %d", bar);
			break;
	}
}

static draw_digit(cairo_t *cr, int digit, gdouble x_offset, gdouble y_offset)
{
	int i;

	for(i = 0; i < 7; i++)
	{
		if( bars_for_digit[digit][i] == 1 )
		{
			draw_bar(cr, i, x_offset, y_offset);
		}
	}  
}


static gboolean on_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	cairo_t *cr;
	gdouble x_offset;
	gint i;
	gint first_digit, second_digit;
	static gint draw_column = 1; 
	static gint count;
	cr = gdk_cairo_create (widget->window);

	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	cairo_set_line_width(cr, 0);

	x_offset = linewidth;

	//hours
	first_digit = time_tm.tm_hour / 10;
	second_digit = time_tm.tm_hour % 10;
	draw_digit(cr, first_digit, x_offset, 0);
	x_offset += x_delta + 0.5*linewidth;
	draw_digit(cr, second_digit, x_offset, 0);
	x_offset += x_delta;

	//column
	if(draw_column)
	{
  	cairo_rectangle(cr, x_offset, vert_bar_length, linewidth, linewidth);
  	cairo_rectangle(cr, x_offset, 1.5 * vert_bar_length, linewidth, linewidth);
		draw_column = 0;	
	}
	else
	{
		draw_column = 1;
	}	
	x_offset += column_width;

  //minutes
	first_digit = time_tm.tm_min / 10;
	second_digit = time_tm.tm_min % 10;
	draw_digit(cr, first_digit, x_offset, 0);
	x_offset += x_delta + 0.5*linewidth;
	draw_digit(cr, second_digit, x_offset, 0);

	cairo_stroke (cr);

	cairo_destroy(cr);
	return TRUE;
}

static gboolean update_time(gpointer data)
{
	GtkWidget *widget;
	GdkRegion *region;
	time_t time_secs;

	widget = GTK_WIDGET(data);	

	if (!widget->window)
		return FALSE;

	time( &time_secs );
	localtime_r (&time_secs, &time_tm);

	region = gdk_drawable_get_clip_region (widget->window);
  gdk_window_invalidate_region (widget->window, region, TRUE);
  gdk_window_process_updates (widget->window, TRUE);
  gdk_region_destroy (region);

	return TRUE;
}

static gboolean calculate_sizes_and_offsets(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
	gint w_width, w_height;
	static gint count = 0;

	//see bar-diagram.dia to make sense out of these numbers

	gtk_window_get_size(GTK_WINDOW(widget), &w_width, &w_height);

	g_debug("calculate_sizes_and_offsets %d", count++);
	//empirically determined 1/25th of the window width
  //provides a nice width for the bars		
	linewidth = w_width/26.0;

	//Column takes 5% of total width
	column_width = 2*linewidth;

	//We have four digits that each take 1/4th
  //of w_width minus column_width. Each box for
  //the digits needs to be one horizontal bar
  //wide.
	x_delta =  6*linewidth;

	//compensate for triangles at endpoints of the
  //digit-bars (see draw_horizontal_bar or draw_vertical_bar)
  //we use x_delta to specify the x_offset for each digit
	hor_bar_length =  3 * linewidth;

	//Each box for the digits needs to hold two vertical bars
	vert_bar_length = (w_height - (4 * linewidth))/2.0;

	x_1_4_offset = -1.25*linewidth;
	x_2_5_offset = hor_bar_length + (0.25*linewidth);
	y_1_2_offset = 1.25*linewidth;
	y_3_offset = 1.5*linewidth + vert_bar_length;
	y_4_5_offset = 2.75*linewidth + vert_bar_length;
	y_6_offset = (2*vert_bar_length) + (3*linewidth);

	return TRUE;
}

int main(int argc, char** argv)
{
	time_t time_secs;	
	GtkWidget *window;

	gtk_init(&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);
	
	time( &time_secs );
	localtime_r (&time_secs, &time_tm);

	g_signal_connect(window, "expose-event", G_CALLBACK(on_expose_event), NULL);
	g_signal_connect(window, "configure-event", G_CALLBACK(calculate_sizes_and_offsets), NULL);
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	create_bars_for_digit();

	calculate_sizes_and_offsets(window, NULL, NULL);

	gtk_widget_set_app_paintable(window, TRUE);
	
	g_timeout_add(1000, update_time, window);
	gtk_widget_show_all(window);

	gtk_main();
}
