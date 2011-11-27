/**
 * \file applets/digitalclock/main.c
 * \brief Creates a window showing the time in digital letters as some LED displays show
 *
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 * \bug hor_bar_length != vert_bar_length. Shouldn't we make these equal?
 */

#include <gtk/gtk.h>
#include <gm_generic.h>
#include <sys/time.h>
#include <math.h>

static GtkWidget *main_window = NULL;
static GtkWidget *hour_window = NULL;
static GtkWidget *minute_window = NULL;
static GtkWidget *colon_window = NULL;
static GtkWidget *date_window = NULL;

static gint timeout_source_id = -1;

/**
 * \struct digit_time
 * \brief holds the time for the digits
 */
struct digit_time {
	gint time; ///< holds the current number shown by the clock
	gint first_digit; ///< holds the integer number for the first digit
	gint second_digit; ///< holds the integer number for the second digit
};

static struct digit_time hours;  ///< represents the current clock value for hours
static struct digit_time minutes; ///< represents the current clock value for minutes

struct _date {
	gint day_first_letter;
	gint day_second_letter;
	gint day_of_month_first_digit;
	gint day_of_month_second_digit;
	gint month_first_letter;	
	gint month_second_letter;
	gint month_third_letter;
	gint year_first_digit;
	gint year_second_digit;
	gint year_third_digit;
	gint year_fourth_digit;
} date;

/**
 * \struct _sizes
 * \brief holds all widths and heights needed to draw the clock
 */
static struct _sizes {
	gdouble w_width; ///< width of the window in which the clock will be drawn
	gdouble w_height; ///< height of the window in which the clock will be drawn
	gdouble linewidth; ///< default sizes.linewidth for horizontal and vertical bars
	gdouble bar_length; ///< length of the bar excluding the top and bottom of the bar
	gdouble triangle_side_length; ///< length of the left and right sides of the triangles at the ends of the bar
} sizes;

/**
 * \struct _offsets
 * \brief holds all offsets for drawing horizontal and vertical bars in a digit. See diagrams calendar-diagram.dia and digits-diagram.dia for explanation
 */
static struct _letter_offsets {
  gdouble digit_x_delta;
  gdouble letter_x_delta;
	gdouble x_0_3_6;
	gdouble x_2_5;
	gdouble x_7_9_11;
	gdouble x_8_10;
	gdouble x_14_18;
	gdouble x_13_17;
	gdouble x_15_19;
	gdouble y_0;
	gdouble y_3; 
	gdouble y_4_5; 
	gdouble y_6; 
	gdouble y_14_15_22_23;
	gdouble y_18_19_26_27;
} letter_offsets;


/**
 * \struct _offsets
 * \brief holds all offsets for drawing horizontal and vertical bars in a digit. See diagrams calendar-diagram.dia and digits-diagram.dia for explanation
 */
static struct _digit_offsets {
  gdouble digit_x_delta;
  gdouble letter_x_delta;
	gdouble x_0_3_6;
	gdouble x_2_5;
	gdouble x_7_9_11;
	gdouble x_8_10;
	gdouble x_14_18;
	gdouble x_13_17;
	gdouble x_15_19;
	gdouble y_0;
	gdouble y_3; 
	gdouble y_4_5; 
	gdouble y_6; 
	gdouble y_14_15_22_23;
	gdouble y_18_19_26_27;
} digit_offsets;

///< double array to specify which bars should be drawn to display a specific number
static gint digits_bars_on_off[10][7] = {
	{1, 1, 1, 0, 1, 1, 1}, // 0
	{0, 0, 1, 0, 0, 1, 0}, // 1 
	{1, 0, 1, 1, 1, 0, 1}, // 2
	{1, 0, 1, 1, 0, 1, 1}, // 3
	{0, 1, 1, 1, 0, 1, 0}, // 4
	{1, 1, 0, 1, 0, 1, 1}, // 5
	{1, 1, 0, 1, 1, 1, 1}, // 6
	{1, 0, 1, 0, 0, 1, 0}, // 7
	{1, 1, 1, 1, 1, 1, 1}, // 8
	{1, 1, 1, 1, 0, 1, 1}  // 9
};

///< double array to specify which bars should be drawm to display a letter
static gint letters_bars_on_off[26][26] = {
	{0, 2, 3, 4, 5, 6, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //1 a
	{1, 3, 4, 5, 6, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //2 b
	{0, 1, 4, 6, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //3 c
	{2, 3, 4, 5, 6, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //4 d
	{0, 1, 2, 3, 4, 6, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //5 e
	{0, 1, 3, 4, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //6 f
	{0, 1, 2, 3, 5, 6, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //7 g
	{1, 3, 4, 5, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //8 h
	{1, 4, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //9 i
	{2, 4, 5, 6, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //10 j
	{1, 4, 13, 14, 16, 19, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //11 k
	{1, 4, 6, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //12 l
	{0, 1, 4, 2, 7, 8, 10, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //13 m
	{0, 1, 2, 4, 5, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //14 n
	{1, 1, 2, 4, 5, 6, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //15 o
	{0, 1, 2, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //16 p
	{0, 1, 2, 3, 5, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //17 q
	{0, 1, 4, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //18 r
	{0, 1, 3, 5, 6, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //19 s
	{1, 3, 4, 6, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //20 t
	{1, 2, 4, 5, 6, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //21 u
	{1, 8, 16, 19, 25, 26, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //v
	{1, 4, 5, 6, 8, 10, 11, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //w
	{12, 15, 17, 18, 21, 22, 24, 27, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //x
	{1, 2, 3, 5, 6, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //y
	{0, 2, 3, 4, 6, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //z
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

static gboolean calculate_offsets_calendar(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
	//see calendar-diagram.dia to make sense out of these numbers

	sizes.linewidth = sizes.w_width/83.5;

	gtk_widget_set_size_request (date_window, sizes.w_width, sizes.w_height*0.3);

	//compensate for triangles at endpoints of the
  //digit-bars (see draw_horizontal_bar or draw_vertical_bar)
	sizes.bar_length =  2.5 * sizes.linewidth;
	sizes.triangle_side_length = sqrt((0.25 * sizes.linewidth * sizes.linewidth) + (0.25 * sizes.linewidth * sizes.linewidth));

	letter_offsets.letter_x_delta = 9.5*sizes.linewidth;
	letter_offsets.x_0_3_6 = 1.25*sizes.linewidth;
	letter_offsets.x_2_5 = 4*sizes.linewidth;
	letter_offsets.x_7_9_11 = digit_offsets.x_2_5 + digit_offsets.x_0_3_6;
	letter_offsets.x_8_10 = 2 * digit_offsets.x_2_5;
	letter_offsets.x_14_18 = (digit_offsets.x_2_5 - sizes.linewidth)/2 + 0.875*sizes.linewidth;
	letter_offsets.x_15_19 = digit_offsets.x_14_18 + (0.25 * sizes.linewidth);
	letter_offsets.x_13_17 = digit_offsets.x_2_5 - (0.25 *  sizes.linewidth); 
	letter_offsets.y_0 = -digit_offsets.x_0_3_6;
	letter_offsets.y_3 = 0.25*sizes.linewidth + sizes.bar_length;
	letter_offsets.y_4_5 = digit_offsets.y_3 + 1.25*sizes.linewidth;
	letter_offsets.y_6 = digit_offsets.y_4_5 + digit_offsets.y_3;
	letter_offsets.y_14_15_22_23 = digit_offsets.x_15_19 - 1.25 * sizes.linewidth;
	letter_offsets.y_18_19_26_27 = digit_offsets.y_4_5 + digit_offsets.y_14_15_22_23;
	return TRUE;
}

static gboolean calculate_offsets_clock(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
	gint time_passed = 0;
	gfloat digit_width;
	gfloat digit_height;
	gfloat colon_width;

	//see digits-diagram.dia to make sense out of these numbers

	//width is 4 * x_delta + columnwidth = 23.5 * linewidth
  //as we only get the width from gappman we calculate from
  //that the required linewidth.
	sizes.linewidth = sizes.w_width/23.5;

	colon_width = 1.5*sizes.linewidth;

	digit_width = (sizes.w_width - colon_width)/4;
  digit_height = sizes.w_height * 0.7;
	
	gtk_widget_set_size_request (hour_window, digit_width * 2, digit_height);
	gtk_widget_set_size_request (colon_window, colon_width, digit_height);
	gtk_widget_set_size_request (minute_window, digit_width * 2, digit_height);
	gtk_widget_set_size_request (date_window, sizes.w_width, digit_height*0.3);
	gtk_widget_set_size_request (main_window, sizes.w_width, sizes.w_height);

	//compensate for triangles at endpoints of the
  //digit-bars (see draw_horizontal_bar or draw_vertical_bar)
	sizes.bar_length =  2.5 * sizes.linewidth;
	sizes.triangle_side_length = sqrt((0.25 * sizes.linewidth * sizes.linewidth) + (0.25 * sizes.linewidth * sizes.linewidth));

	digit_offsets.digit_x_delta = 5.5*sizes.linewidth;
	digit_offsets.x_0_3_6 = 1.25*sizes.linewidth;
	digit_offsets.x_2_5 = 4*sizes.linewidth;
	digit_offsets.x_7_9_11 = digit_offsets.x_2_5 + digit_offsets.x_0_3_6;
	digit_offsets.x_8_10 = 2 * digit_offsets.x_2_5;
	digit_offsets.x_14_18 = (digit_offsets.x_2_5 - sizes.linewidth)/2 + 0.875*sizes.linewidth;
	digit_offsets.x_15_19 = digit_offsets.x_14_18 + (0.25 * sizes.linewidth);
	digit_offsets.x_13_17 = digit_offsets.x_2_5 - (0.25 *  sizes.linewidth); 
	digit_offsets.y_0 = -digit_offsets.x_0_3_6;
	digit_offsets.y_3 = 0.25*sizes.linewidth + sizes.bar_length;
	digit_offsets.y_4_5 = digit_offsets.y_3 + 1.25*sizes.linewidth;
	digit_offsets.y_6 = digit_offsets.y_4_5 + digit_offsets.y_3;
	digit_offsets.y_14_15_22_23 = digit_offsets.x_15_19 - 1.25 * sizes.linewidth;
	digit_offsets.y_18_19_26_27 = digit_offsets.y_4_5 + digit_offsets.y_14_15_22_23;
	return TRUE;
}

static gboolean draw_diagonal_bar_left(cairo_t *cr, gdouble x, gdouble y)
{
	//draw left triangle
	cairo_move_to(cr, x, y);

	//draw left vertical line
	cairo_rel_line_to(cr, 0, sizes.triangle_side_length);

	//draw bottom line rectangle
	cairo_rel_line_to (cr, 0.37 * sizes.linewidth, 0.37 * sizes.linewidth);

  //draw bottom horizontal line
	cairo_rel_line_to(cr, sizes.triangle_side_length, 0);

	//draw right vertical line
	cairo_rel_line_to(cr, 0, -sizes.triangle_side_length);

	//draw top line rectangle
	cairo_rel_line_to (cr, -0.37 * sizes.linewidth, -0.37 * sizes.linewidth);

	//draw top horizontal line
	cairo_rel_line_to(cr, -sizes.triangle_side_length, 0);

	cairo_close_path(cr);
	cairo_set_line_join (cr, CAIRO_LINE_JOIN_ROUND);
	cairo_fill(cr);
}

static gboolean draw_diagonal_bar_right(cairo_t *cr, gdouble x, gdouble y)
{
	//draw left triangle
	cairo_move_to(cr, x, y);

	cairo_rel_line_to(cr, 0, sizes.triangle_side_length);

	//draw bottom line rectangle
	cairo_rel_line_to (cr, -0.37 * sizes.linewidth, 0.37 * sizes.linewidth);

	//draw bottom horizontal line
	cairo_rel_line_to(cr, -sizes.triangle_side_length, 0);

	//draw right vertical line
	cairo_rel_line_to(cr, 0, -sizes.triangle_side_length);

	//draw top line rectangle
	cairo_rel_line_to (cr, 0.37 * sizes.linewidth, -0.37 * sizes.linewidth);

	//draw top horizontal line
	cairo_rel_line_to(cr, sizes.triangle_side_length, 0);

	cairo_close_path(cr);

	cairo_set_line_join (cr, CAIRO_LINE_JOIN_ROUND);
	cairo_fill(cr);
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
	cairo_rectangle(cr, x, y, sizes.bar_length, sizes.linewidth);

	//draw right triangle
	cairo_move_to(cr, x+sizes.bar_length, y);
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
	cairo_rectangle(cr, x, y, sizes.linewidth, sizes.bar_length);

	//draw bottom triangle
	cairo_move_to(cr, x, y+sizes.bar_length);
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
			draw_horizontal_bar(cr, x + digit_offsets.x_0_3_6, y + digit_offsets.y_0);
			break;
		case 1:
			draw_vertical_bar(cr, x , y);
			break;
		case 2:
			draw_vertical_bar(cr, x + digit_offsets.x_2_5, y);
			break;
		case 3:
			draw_horizontal_bar(cr, x + digit_offsets.x_0_3_6, y + digit_offsets.y_3);
			break;
		case 4:
			draw_vertical_bar(cr, x, y + digit_offsets.y_4_5);
			break;
		case 5:
			draw_vertical_bar(cr, x + digit_offsets.x_2_5, y + digit_offsets.y_4_5);
			break;
		case 6:
			draw_horizontal_bar(cr, x + digit_offsets.x_0_3_6, y + digit_offsets.y_6);
			break;
		case 7:
			draw_horizontal_bar(cr, x + digit_offsets.x_0_3_6 + digit_offsets.x_2_5, y + digit_offsets.y_6);
			break;
		case 8:
			draw_horizontal_bar(cr, x + digit_offsets.x_0_3_6 + digit_offsets.x_2_5, y + digit_offsets.y_6);
			break;
		case 9:
			draw_horizontal_bar(cr, x + digit_offsets.x_0_3_6 + digit_offsets.x_2_5, y + digit_offsets.y_6);
			break;
		case 10:
			draw_horizontal_bar(cr, x + digit_offsets.x_0_3_6 + digit_offsets.x_2_5, y + digit_offsets.y_6);
			break;
		case 11:
			draw_horizontal_bar(cr, x + digit_offsets.x_0_3_6 + digit_offsets.x_2_5, y + digit_offsets.y_6);
			break;
		case 12:
			draw_diagonal_bar_left(cr, x + digit_offsets.x_0_3_6, y);
			break;
		case 13:
			draw_diagonal_bar_right(cr, x + digit_offsets.x_13_17, y);
			break;
		case 14:
			draw_diagonal_bar_right(cr, x + digit_offsets.x_14_18, y + digit_offsets.y_14_15_22_23);
			break;
		case 15:
			draw_diagonal_bar_left(cr, x + digit_offsets.x_15_19, y + digit_offsets.y_14_15_22_23);
			break;
		case 16:
			draw_diagonal_bar_left(cr, x + digit_offsets.x_0_3_6, y + digit_offsets.y_4_5);
			break;
		case 17:
			draw_diagonal_bar_right(cr, x + digit_offsets.x_13_17, y + digit_offsets.y_4_5);
			break;
		case 18:
			draw_diagonal_bar_right(cr, x + digit_offsets.x_14_18, y + digit_offsets.y_18_19_26_27);
			break;
		case 19:
			draw_diagonal_bar_left(cr, x + digit_offsets.x_15_19, y + digit_offsets.y_18_19_26_27);
			break;
		case 20:
			draw_diagonal_bar_left(cr, x + digit_offsets.x_0_3_6 + digit_offsets.x_2_5, y);
			break;
		case 21:
			draw_diagonal_bar_right(cr, x + digit_offsets.x_13_17 + digit_offsets.x_2_5, y);
			break;
		case 22:
			draw_diagonal_bar_right(cr, x + digit_offsets.x_14_18 + digit_offsets.x_2_5, y + digit_offsets.y_14_15_22_23);
			break;
		case 23:
			draw_diagonal_bar_left(cr, x + digit_offsets.x_15_19 + digit_offsets.x_2_5, y + digit_offsets.y_14_15_22_23);
			break;
		case 24:
			draw_diagonal_bar_left(cr, x + digit_offsets.x_0_3_6 + digit_offsets.x_2_5, y + digit_offsets.y_4_5);
			break;
		case 25:
			draw_diagonal_bar_right(cr, x + digit_offsets.x_13_17 + digit_offsets.x_2_5, y + digit_offsets.y_4_5);
			break;
		case 26:
			draw_diagonal_bar_right(cr, x + digit_offsets.x_14_18 + digit_offsets.x_2_5, y + digit_offsets.y_18_19_26_27);
			break;
		case 27:
			draw_diagonal_bar_left(cr, x + digit_offsets.x_15_19 + digit_offsets.x_2_5, y + digit_offsets.y_18_19_26_27);
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
		if( digits_bars_on_off[digit][i] == 1 )
		{
			draw_bar(cr, i, x_offset, y_offset);
		}
	}  
}

static draw_letter(cairo_t *cr, int letter, gdouble x_offset, gdouble y_offset)
{
	gint i;
	gint bar;
	for(i = 0; i < 26; i++)
	{
		bar = letters_bars_on_off[letter][i];
		if( bar == -1 )
		{
			break;
		}
		g_debug("letter %d drawing bar %d", letter, i);
		draw_bar(cr, bar, x_offset, y_offset);
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

	draw_digit(cr, hours.first_digit, 0, -digit_offsets.y_0);
	draw_digit(cr, hours.second_digit, digit_offsets.digit_x_delta + 0.5*sizes.linewidth, -digit_offsets.y_0);

	cairo_stroke (cr);
	cairo_destroy(cr);
	
	return TRUE;
}

static gboolean colon_on_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	cairo_t *cr;
	static gint draw_colon = 1; 
	GtkStyle *rc_style;
	gint time_passed = 0;

	if(draw_colon)
	{
		cr = gdk_cairo_create (widget->window);

		rc_style = gtk_rc_get_style(widget);

		cairo_set_source_rgb (cr, 
			rc_style->fg[0].red,
			rc_style->fg[0].green,
			rc_style->fg[0].blue);

		cairo_set_line_width(cr, 0);

  	cairo_rectangle(cr, 0.25*sizes.linewidth, digit_offsets.y_3 - sizes.linewidth - digit_offsets.y_0, sizes.linewidth, sizes.linewidth);
  	cairo_rectangle(cr, 0.25*sizes.linewidth, digit_offsets.y_4_5 - digit_offsets.y_0, sizes.linewidth, sizes.linewidth);

		cairo_fill (cr);
		cairo_destroy(cr);

		draw_colon = 0;	
	}
	else
	{
		draw_colon = 1;
	}	
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

	draw_digit(cr, minutes.first_digit, 0, -digit_offsets.y_0);
	draw_digit(cr, minutes.second_digit, digit_offsets.digit_x_delta + 0.5*sizes.linewidth, -digit_offsets.y_0);

	cairo_stroke (cr);
	cairo_destroy(cr);

	return TRUE;
}

static gboolean date_on_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data)
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

	draw_letter(cr, date.day_first_letter, 0, -letter_offsets.y_0);
	draw_letter(cr, date.day_second_letter, letter_offsets.digit_x_delta, -letter_offsets.y_0);

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
	
		region = gdk_drawable_get_clip_region (minute_window->window);
 		gdk_window_invalidate_region (minute_window->window, region, TRUE);
 		gdk_window_process_updates (minute_window->window, TRUE);
 		gdk_region_destroy (region);
	}

	region = gdk_drawable_get_clip_region (colon_window->window);
 	gdk_window_invalidate_region (colon_window->window, region, TRUE);
 	gdk_window_process_updates (colon_window->window, TRUE);
 	gdk_region_destroy (region);
	return TRUE;
}



/**
* \brief converts a character to the correct arraynumber in letter_bars array
* \return gint
*/
gint convert_to_letterbars_number(gchar c)
{
	int number = (int) c - 'a';
	if (number < 0 && number > 26)
		return 0;
	
	return number;
}

/**
* \brief initializes the digital clock
* \return GM_SUCCESS
*/
G_MODULE_EXPORT int gm_module_init()
{
	time_t time_secs;
	struct tm cur_time;
	gchar *datestring;
	GtkWidget *hbox;

	main_window = gtk_vbox_new(FALSE, 0);
	hbox = gtk_hbox_new(FALSE, 0);

	hour_window = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(hbox), hour_window);

	colon_window = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(hbox), colon_window);
	
	minute_window = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(hbox), minute_window);

	gtk_container_add(GTK_CONTAINER(main_window), hbox);

	date_window = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(main_window), date_window);

	g_signal_connect(minute_window, "expose-event", G_CALLBACK(minute_on_expose_event), NULL);
	g_signal_connect(hour_window, "expose-event", G_CALLBACK(hour_on_expose_event), NULL);
	g_signal_connect(colon_window, "expose-event", G_CALLBACK(colon_on_expose_event), NULL);
	g_signal_connect(date_window, "expose-event", G_CALLBACK(date_on_expose_event), NULL);

	g_signal_connect(minute_window, "configure-event", G_CALLBACK(calculate_offsets_clock), NULL);
	g_signal_connect(date_window, "configure-event", G_CALLBACK(calculate_offsets_calendar), NULL);

	time( &time_secs );
	localtime_r (&time_secs, &cur_time);

	hours.time = cur_time.tm_hour;
	hours.first_digit = cur_time.tm_hour / 10;
	hours.second_digit = cur_time.tm_hour % 10;

	minutes.time = cur_time.tm_min;
	minutes.first_digit = cur_time.tm_min / 10;
	minutes.second_digit = cur_time.tm_min % 10;

	datestring = ctime(&time_secs);
	g_debug("Date: %s\n", datestring);
	date.day_first_letter = convert_to_letterbars_number(tolower(datestring[0]));
	date.day_second_letter = convert_to_letterbars_number(datestring[1]);
	date.month_first_letter = convert_to_letterbars_number(tolower(datestring[4]));
	date.month_second_letter = convert_to_letterbars_number(datestring[5]);
	date.month_third_letter = convert_to_letterbars_number(datestring[6]);

	// we assume this software will not last 7989 years
	date.year_first_digit = cur_time.tm_year / 1000;
	date.year_second_digit = cur_time.tm_year % 1000 / 100;
	date.year_third_digit = cur_time.tm_year % 100 / 10;
	date.year_fourth_digit = cur_time.tm_year % 10;

	return GM_SUCCESS;
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
