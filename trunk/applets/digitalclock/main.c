/**
 * \file applets/digitalclock/main.c
 * \brief Creates a window showing the time in digital letters as some LED displays show
 *
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 *
 * \todo see if drawing of digits can be done only once a minute, instead of each second.
 * 			 depending on the digits drawn it takes 1000 to 2000 microseconds.
 * \todo add day/month/year
 * \todo add calendar
 * \todo Add support to use images as horizontal and vertical bars
 * \todo Struct sizes now has both w_width, w_height and digit_width, digit_height. We can replace w_widht and w_height with the digit versions.
 * \todo We now use seperate drawing area's for hours, minutes, and the column. Should we use regions for this?
 */

#include <gtk/gtk.h>
#include <gm_generic.h>
#include <sys/time.h>

static GtkWidget *main_window = NULL;
static GtkWidget *hour_window = NULL;
static GtkWidget *minute_window = NULL;
static GtkWidget *column_window = NULL;

static gint timeout_source_id = -1;

struct digit_time {
	gint time; ///< holds the current number shown by the clock
	gint first_digit; ///< holds the integer number for the first digit
	gint second_digit; ///< holds the integer number for the second digit
};

static struct digit_time hours;  ///< represents the current clock value for hours
static struct digit_time minutes; ///< represents the current clock value for minutes

static struct _sizes {
	gdouble digit_width; ///< width of the window in which the clock will be drawn
	gdouble digit_height; ///< height of the window in which the clock will be drawn
	gdouble w_width; ///< width of the window in which the clock will be drawn
	gdouble w_height; ///< height of the window in which the clock will be drawn
	gdouble column_width; ///< holds the width of the column which is placed between the hour andd minute digits
	gdouble linewidth; ///< default sizes.linewidth for horizontal and vertical bars
	gdouble vert_bar_length; ///< length of the vertical bar
	gdouble hor_bar_length; ///< length of the horizontal bar
} sizes;

static struct _offsets {
  gdouble x_delta; ///< the amount of space we should move horizontally to start drawing the next digit
	gdouble x_0_3_6; ///< horizontal offset calculated from the top left corner of the digit to draw the bars at positions 0, 3, and 6
	gdouble x_2_5; ///< horizontal offset calculated from the top left corner of the digit to draw the bars at positions 2 and 5
	gdouble y_0; ///< vertical offset calculated from the top left corner of the digit to draw the bar at positions 0
	gdouble y_3; ///< vertical offset calculated from the top left corner of the digit to draw the bar at position 3
	gdouble y_4_5; ///< vertical offset calculated from the top left corner of the digit to draw the bars at positions 4 and 5
	gdouble y_6; ///< vertical offset calculated from the top left corner of the digit to draw the bar at position 6
} offsets;

//DOES THIS REALLY NEED TO BE GLOBAL?
///< double array to specify which bars should be drawn to display a specific number
static gint bars_on_off[10][7] = {
	{1, 1, 1, 0, 1, 1, 1},
	{0, 0, 1, 0, 0, 1, 0},
	{1, 0, 1, 1, 1, 0, 1},
	{1, 0, 1, 1, 0, 1, 1},
	{0, 1, 1, 1, 0, 1, 0},
	{1, 1, 0, 1, 0, 1, 1},
	{1, 1, 0, 1, 1, 1, 1},
	{1, 0, 1, 0, 0, 1, 0},
	{1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 0, 1, 1} 
};

static void measure_time(int *prev_microseconds)
{
	struct timeval time_tv;
	
	if(*prev_microseconds)
	{
		gettimeofday( &time_tv, NULL );
		g_debug("Microseconds passed: %d", (int) time_tv.tv_usec - *prev_microseconds);
		*prev_microseconds = (int) time_tv.tv_usec;
	}	
	gettimeofday( &time_tv, NULL );
	*prev_microseconds = (int) time_tv.tv_usec;
}


static gboolean draw_horizontal_bar(cairo_t *cr, gdouble x, gdouble y)
{

	gdouble halflinewidth = (sizes.linewidth)/2.0;

	//draw left triangle
	cairo_move_to(cr, x, y);
	cairo_rel_line_to(cr, -halflinewidth, halflinewidth);
	cairo_rel_line_to(cr, halflinewidth, halflinewidth);
	cairo_close_path(cr);

	//draw vertical line
	cairo_rectangle(cr, x, y, sizes.hor_bar_length, sizes.linewidth);

	//draw right triangle
	cairo_move_to(cr, x+sizes.hor_bar_length, y);
	cairo_rel_line_to(cr, halflinewidth, halflinewidth);
	cairo_rel_line_to(cr, -halflinewidth, halflinewidth);
	cairo_close_path(cr);

	cairo_set_line_join (cr, CAIRO_LINE_JOIN_ROUND);
	cairo_fill(cr);
}


static gboolean draw_vertical_bar(cairo_t *cr, gdouble x, gdouble y)
{

	double halflinewidth = sizes.linewidth/2;

	//draw top triangle
	cairo_move_to(cr, x, y);
	cairo_rel_line_to(cr, halflinewidth, -halflinewidth);
	cairo_rel_line_to(cr, halflinewidth, halflinewidth);
	cairo_close_path(cr);

	//draw vertical line
	cairo_rectangle(cr, x, y, sizes.linewidth, sizes.vert_bar_length);

	//draw bottom triangle
	cairo_move_to(cr, x, y+sizes.vert_bar_length);
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
			draw_horizontal_bar(cr, x + offsets.x_0_3_6, y + offsets.y_0);
			break;
		case 1:
			draw_vertical_bar(cr, x , y);
			break;
		case 2:
			draw_vertical_bar(cr, x + offsets.x_2_5, y);
			break;
		case 3:
			draw_horizontal_bar(cr, x + offsets.x_0_3_6, y + offsets.y_3);
			break;
		case 4:
			draw_vertical_bar(cr, x, y + offsets.y_4_5);
			break;
		case 5:
			draw_vertical_bar(cr, x + offsets.x_2_5, y + offsets.y_4_5);
			break;
		case 6:
			draw_horizontal_bar(cr, x + offsets.x_0_3_6, y + offsets.y_6);
			break;
		default:
			g_warning("Sorry sir, but I have no knowledge of bar number %d", bar);
			break;
	}
}

static draw_digit(cairo_t *cr, int digit, gdouble x_offset, gdouble y_offset)
{
	int i;
	gint time_passed = 0;

	for(i = 0; i < 7; i++)
	{
		if( bars_on_off[digit][i] == 1 )
		{
			draw_bar(cr, i, x_offset, y_offset);
		}
	}  
}

static gboolean hour_on_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	cairo_t *cr;
	GtkStyle *rc_style;
	gint time_passed = 0;

	cr = gdk_cairo_create (widget->window);

	rc_style = gtk_rc_get_style(widget);

	cairo_set_source_rgb (cr, 
		rc_style->fg[0].red,
		rc_style->fg[0].green,
		rc_style->fg[0].blue);

	cairo_set_line_width(cr, 0);

	draw_digit(cr, hours.first_digit, 0, -offsets.y_0);
	draw_digit(cr, hours.second_digit, offsets.x_delta + 0.5*sizes.linewidth, -offsets.y_0);

	cairo_stroke (cr);
	cairo_destroy(cr);
	
	return TRUE;
}

static gboolean column_on_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	cairo_t *cr;
	static gint draw_column = 1; 
	GtkStyle *rc_style;
	gint time_passed = 0;

	cr = gdk_cairo_create (widget->window);

	rc_style = gtk_rc_get_style(widget);

	cairo_set_source_rgb (cr, 
		rc_style->fg[0].red,
		rc_style->fg[0].green,
		rc_style->fg[0].blue);

	cairo_set_line_width(cr, 0);


	//column
	if(draw_column)
	{
  	cairo_rectangle(cr, 0.5*sizes.linewidth, offsets.y_3 - sizes.linewidth - offsets.y_0, sizes.linewidth, sizes.linewidth);
  	cairo_rectangle(cr, 0.5*sizes.linewidth, offsets.y_4_5 - offsets.y_0, sizes.linewidth, sizes.linewidth);
		draw_column = 0;	
	}
	else
	{
		draw_column = 1;
	}	

	cairo_fill (cr);
	cairo_destroy(cr);

	return TRUE;
}

static gboolean minute_on_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	cairo_t *cr;
	GtkStyle *rc_style;
	gint time_passed = 0;

	cr = gdk_cairo_create (widget->window);

	rc_style = gtk_rc_get_style(widget);

	cairo_set_source_rgb (cr, 
		rc_style->fg[0].red,
		rc_style->fg[0].green,
		rc_style->fg[0].blue);

	cairo_set_line_width(cr, 0);

	draw_digit(cr, minutes.first_digit, 0, -offsets.y_0);
	draw_digit(cr, minutes.second_digit, offsets.x_delta + 0.5*sizes.linewidth, -offsets.y_0);

	cairo_stroke (cr);
	cairo_destroy(cr);

	return TRUE;
}


static gboolean update_time(gpointer data)
{
	GdkRegion *region;
	time_t time_secs;
	struct tm cur_time;

	time( &time_secs );
	localtime_r (&time_secs, &cur_time);

	if( hours.time != cur_time.tm_hour )
	{
		hours.first_digit = cur_time.tm_hour / 10;
		hours.second_digit = cur_time.tm_hour % 10;

		hours.time = cur_time.tm_hour;

		//Invalidate hour window
		region = gdk_drawable_get_clip_region (hour_window->window);
 		gdk_window_invalidate_region (hour_window->window, region, TRUE);
 		gdk_window_process_updates (hour_window->window, TRUE);
 		gdk_region_destroy (region);
	}

	if( minutes.time != cur_time.tm_min )
	{
  	//minutes
		minutes.first_digit = cur_time.tm_min / 10;
		minutes.second_digit = cur_time.tm_min % 10;

		minutes.time = cur_time.tm_min;
	
		//TODO: invalidate minute window
		region = gdk_drawable_get_clip_region (minute_window->window);
 		gdk_window_invalidate_region (minute_window->window, region, TRUE);
 		gdk_window_process_updates (minute_window->window, TRUE);
 		gdk_region_destroy (region);
	}

	//TODO: invalidate column window	
	region = gdk_drawable_get_clip_region (column_window->window);
 	gdk_window_invalidate_region (column_window->window, region, TRUE);
 	gdk_window_process_updates (column_window->window, TRUE);
 	gdk_region_destroy (region);
	return TRUE;
}

static gboolean calculate_offsets(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
	gint time_passed = 0;
	
	//see bar-diagram.dia to make sense out of these numbers

	//empirically determined 1/26th of the window width
  //provides a nice width for the bars		
	sizes.linewidth = sizes.w_width/26.0;

	//Column takes 5% of total width
	sizes.column_width = 2.0*sizes.linewidth;

	sizes.digit_width = (sizes.w_width - sizes.column_width)/4;
  sizes.digit_height = sizes.w_height;

	//We have four digits that each take 1/4th
  //of sizes.w_width minus sizes.column_width. Each box for
  //the digits needs to be one horizontal bar
  //wide.
	offsets.x_delta =  6.0*sizes.linewidth;

	//compensate for triangles at endpoints of the
  //digit-bars (see draw_horizontal_bar or draw_vertical_bar)
  //we use offset.x_delta to specify the x_offset for each digit
	sizes.hor_bar_length =  3.0 * sizes.linewidth;

	//Each box for the digits needs to hold two vertical bars
	sizes.vert_bar_length = (sizes.w_height - (4.0 * sizes.linewidth))/2.0;

	offsets.x_0_3_6 = 1.25*sizes.linewidth;
	offsets.x_2_5 = sizes.hor_bar_length + (1.5*sizes.linewidth);
	offsets.y_0 = -offsets.x_0_3_6;
	offsets.y_3 = 0.25*sizes.linewidth + sizes.vert_bar_length;
	offsets.y_4_5 = offsets.y_3 + 1.25*sizes.linewidth;
	offsets.y_6 = offsets.y_4_5 + offsets.y_3;

	gtk_widget_set_size_request (hour_window, sizes.digit_width * 2, sizes.digit_height);
	gtk_widget_set_size_request (column_window, sizes.column_width, sizes.digit_height);
	gtk_widget_set_size_request (minute_window, sizes.digit_width * 2, sizes.digit_height);
	gtk_widget_set_size_request (main_window, sizes.w_width, sizes.digit_height);
	return TRUE;
}

/**
* \brief initializes the digital clock
* \return GM_SUCCES
*/
G_MODULE_EXPORT int gm_module_init()
{
	time_t time_secs;
	struct tm cur_time;

	main_window = gtk_hbox_new(FALSE, 0);

	hour_window = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(main_window), hour_window);

	column_window = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(main_window), column_window);
	
	minute_window = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(main_window), minute_window);

	g_signal_connect(minute_window, "expose-event", G_CALLBACK(minute_on_expose_event), NULL);
	g_signal_connect(hour_window, "expose-event", G_CALLBACK(hour_on_expose_event), NULL);
	g_signal_connect(column_window, "expose-event", G_CALLBACK(column_on_expose_event), NULL);

	g_signal_connect(minute_window, "configure-event", G_CALLBACK(calculate_offsets), NULL);

	time( &time_secs );
	localtime_r (&time_secs, &cur_time);

	hours.time = cur_time.tm_hour;
	hours.first_digit = cur_time.tm_hour / 10;
	hours.second_digit = cur_time.tm_hour % 10;

	minutes.time = cur_time.tm_min;
	minutes.first_digit = cur_time.tm_min / 10;
	minutes.second_digit = cur_time.tm_min % 10;

	return GM_SUCCES;
}

/**
* \brief starts the digital clock updating it each second
*/
G_MODULE_EXPORT void gm_module_start()
{
	gtk_widget_show_all(main_window);
	timeout_source_id = g_timeout_add(1000, update_time, NULL);
}

/**
* \brief stops the digital clock and destroys the window
*/
G_MODULE_EXPORT int gm_module_stop()
{
	if ( timeout_source_id != -1 )
	{
		g_source_remove(timeout_source_id);
	}
}

/**
* \brief returns the window that holds the digital clock
* \return GtkWidget pointer
*/
G_MODULE_EXPORT GtkWidget* gm_module_get_widget()
{
	return main_window;
}

/**
* \brief sets the maximum width and height of the window that will hold the digital clock
*/
G_MODULE_EXPORT void gm_module_set_icon_size(int width, int height)
{
	sizes.w_width = width;
	sizes.w_height = height;
}
