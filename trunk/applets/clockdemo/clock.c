/**
 * clock.c
 *
 * A GTK+ widget that implements a clock face
 *
 * (c) 2005-2006, Davyd Madeley
 *
 * Authors:
 *   Davyd Madeley  <davyd@madeley.id.au>
 */

#include <gtk/gtk.h>
#include <math.h>
#include <time.h>

#include "clock.h"
#include "clock-marshallers.h"

#define EGG_CLOCK_FACE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), EGG_TYPE_CLOCK_FACE, EggClockFacePrivate))

G_DEFINE_TYPE (EggClockFace, egg_clock_face, GTK_TYPE_DRAWING_AREA);

static gboolean egg_clock_face_expose (GtkWidget *clock, GdkEventExpose *event);
static gboolean egg_clock_face_button_press (GtkWidget *clock,
        GdkEventButton *event);
static gboolean egg_clock_face_button_release (GtkWidget *clock,
        GdkEventButton *event);
static gboolean egg_clock_face_motion_notify (GtkWidget *clock,
        GdkEventMotion *event);
static gboolean egg_clock_face_update (gpointer data);

typedef struct _EggClockFacePrivate EggClockFacePrivate;

struct _EggClockFacePrivate
{
    struct tm time;	/* the time on the clock face */
    int minute_offset; /* the offset of the minutes hand */

    gboolean dragging; /* true if the interface is being dragged */
};

enum
{
    TIME_CHANGED,
    LAST_SIGNAL
};

static guint egg_clock_face_signals[LAST_SIGNAL] = { 0 };

static void
egg_clock_face_class_init (EggClockFaceClass *class)
{
    GObjectClass *obj_class;
    GtkWidgetClass *widget_class;

    obj_class = G_OBJECT_CLASS (class);
    widget_class = GTK_WIDGET_CLASS (class);

    /* GtkWidget signals */
    widget_class->expose_event = egg_clock_face_expose;
    widget_class->button_press_event = egg_clock_face_button_press;
    widget_class->button_release_event = egg_clock_face_button_release;
    widget_class->motion_notify_event = egg_clock_face_motion_notify;

    /* EggClockFace signals */
    egg_clock_face_signals[TIME_CHANGED] = g_signal_new (
                                               "time-changed",
                                               G_OBJECT_CLASS_TYPE (obj_class),
                                               G_SIGNAL_RUN_FIRST,
                                               G_STRUCT_OFFSET (EggClockFaceClass, time_changed),
                                               NULL, NULL,
                                               _clock_marshal_VOID__INT_INT,
                                               G_TYPE_NONE, 2,
                                               G_TYPE_INT,
                                               G_TYPE_INT);

    g_type_class_add_private (obj_class, sizeof (EggClockFacePrivate));
}

static void
egg_clock_face_init (EggClockFace *clock)
{
    gtk_widget_add_events (GTK_WIDGET (clock),
                           GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK |
                           GDK_POINTER_MOTION_MASK);

    egg_clock_face_update (clock);

    /* update the clock once a second */
    g_timeout_add (1000, egg_clock_face_update, clock);
}

static void
draw (GtkWidget *clock, cairo_t *cr)
{
    EggClockFacePrivate *priv;
    double x, y;
    double radius;
    int i;
    int hours, minutes, seconds;

    priv = EGG_CLOCK_FACE_GET_PRIVATE (clock);

    x = clock->allocation.width / 2;
    y = clock->allocation.height / 2;
    radius = MIN (clock->allocation.width / 2,
                  clock->allocation.height / 2) - 5;

    /* clock back */
    cairo_arc (cr, x, y, radius, 0, 2 * M_PI);
    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_fill_preserve (cr);
    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_stroke (cr);

    /* clock ticks */
    for (i = 0; i < 12; i++)
    {
        int inset;

        cairo_save (cr); /* stack-pen-size */

        if (i % 3 == 0)
        {
            inset = 0.2 * radius;
        }
        else
        {
            inset = 0.1 * radius;
            cairo_set_line_width (cr, 0.5 *
                                  cairo_get_line_width (cr));
        }

        cairo_move_to (cr,
                       x + (radius - inset) * cos (i * M_PI / 6),
                       y + (radius - inset) * sin (i * M_PI / 6));
        cairo_line_to (cr,
                       x + radius * cos (i * M_PI / 6),
                       y + radius * sin (i * M_PI / 6));
        cairo_stroke (cr);
        cairo_restore (cr); /* stack-pen-size */
    }

    /* clock hands */
    hours = priv->time.tm_hour;
    minutes = priv->time.tm_min + priv->minute_offset;
    seconds = priv->time.tm_sec;
    /* hour hand:
     * the hour hand is rotated 30 degrees (pi/6 r) per hour +
     * 1/2 a degree (pi/360 r) per minute
     */
    cairo_save (cr);
    cairo_set_line_width (cr, 2.5 * cairo_get_line_width (cr));
    cairo_move_to (cr, x, y);
    cairo_line_to (cr, x + radius / 2 * sin (M_PI / 6 * hours +
                   M_PI / 360 * minutes),
                   y + radius / 2 * -cos (M_PI / 6 * hours +
                                          M_PI / 360 * minutes));
    cairo_stroke (cr);
    cairo_restore (cr);
    /* minute hand:
     * the minute hand is rotated 6 degrees (pi/30 r) per minute
     */
    cairo_move_to (cr, x, y);
    cairo_line_to (cr, x + radius * 0.75 * sin (M_PI / 30 * minutes),
                   y + radius * 0.75 * -cos (M_PI / 30 * minutes));
    cairo_stroke (cr);
    /* seconds hand:
     * operates identically to the minute hand
     */
    cairo_save (cr);
    cairo_set_source_rgb (cr, 1, 0, 0); /* red */
    cairo_move_to (cr, x, y);
    cairo_line_to (cr, x + radius * 0.7 * sin (M_PI / 30 * seconds),
                   y + radius * 0.7 * -cos (M_PI / 30 * seconds));
    cairo_stroke (cr);
    cairo_restore (cr);
}

static gboolean
egg_clock_face_expose (GtkWidget *clock, GdkEventExpose *event)
{
    cairo_t *cr;

    /* get a cairo_t */
    cr = gdk_cairo_create (clock->window);

    cairo_rectangle (cr,
                     event->area.x, event->area.y,
                     event->area.width, event->area.height);
    cairo_clip (cr);

    draw (clock, cr);

    cairo_destroy (cr);

    return FALSE;
}

static gboolean
egg_clock_face_button_press (GtkWidget *clock, GdkEventButton *event)
{
    EggClockFacePrivate *priv;
    int minutes;
    double lx, ly;
    double px, py;
    double u, d2;

    priv = EGG_CLOCK_FACE_GET_PRIVATE (clock);

    minutes = priv->time.tm_min + priv->minute_offset;

    /* From
     * http://mathworld.wolfram.com/Point-LineDistance2-Dimensional.html
     */
    px = event->x - clock->allocation.width / 2;
    py = clock->allocation.height / 2 - event->y;
    lx = sin (M_PI / 30 * minutes);
    ly = cos (M_PI / 30 * minutes);
    u = lx * px + ly * py;

    /* on opposite side of origin */
    if (u < 0) return FALSE;

    d2 = pow (px - u * lx, 2) + pow (py - u * ly, 2);

    if (d2 < 25) /* 5 pixels away from the line */
    {
        priv->dragging = TRUE;
        g_print ("got minute hand\n");
    }

    return FALSE;
}

static void
egg_clock_face_redraw_canvas (EggClockFace *clock)
{
    GtkWidget *widget;
    GdkRegion *region;

    widget = GTK_WIDGET (clock);

    if (!widget->window) return;

    region = gdk_drawable_get_clip_region (widget->window);
    /* redraw the cairo canvas completely by exposing it */
    gdk_window_invalidate_region (widget->window, region, TRUE);
    gdk_window_process_updates (widget->window, TRUE);

    gdk_region_destroy (region);
}

static emit_time_changed_signal (EggClockFace *clock, int x, int y)
{
    EggClockFacePrivate *priv;
    double phi;
    int hour, minute;

    priv = EGG_CLOCK_FACE_GET_PRIVATE (clock);

    /* decode the minute hand */
    /* normalise the coordinates around the origin */
    x -= GTK_WIDGET (clock)->allocation.width / 2;
    y -= GTK_WIDGET (clock)->allocation.height / 2;

    /* phi is a bearing from north clockwise, use the same geometry as we
     * did to position the minute hand originally */
    phi = atan2 (x, -y);
    if (phi < 0)
        phi += M_PI * 2;

    hour = priv->time.tm_hour;
    minute = phi * 30 / M_PI;

    /* update the offset */
    priv->minute_offset = minute - priv->time.tm_min;
    egg_clock_face_redraw_canvas (clock);

    g_signal_emit (clock,
                   egg_clock_face_signals[TIME_CHANGED],
                   0,
                   hour, minute);
}

static gboolean
egg_clock_face_motion_notify (GtkWidget *clock, GdkEventMotion *event)
{
    EggClockFacePrivate *priv;
    int x, y;

    priv = EGG_CLOCK_FACE_GET_PRIVATE (clock);

    if (priv->dragging)
    {
        emit_time_changed_signal (EGG_CLOCK_FACE (clock),
                                  event->x, event->y);
    }
}

static gboolean
egg_clock_face_button_release (GtkWidget *clock, GdkEventButton *event)
{
    EggClockFacePrivate *priv;

    priv = EGG_CLOCK_FACE_GET_PRIVATE (clock);

    if (priv->dragging)
    {
        priv = EGG_CLOCK_FACE_GET_PRIVATE (clock);
        priv->dragging = FALSE;
        emit_time_changed_signal (EGG_CLOCK_FACE (clock),
                                  event->x, event->y);
    }

    return FALSE;
}

static gboolean
egg_clock_face_update (gpointer data)
{
    EggClockFace *clock;
    EggClockFacePrivate *priv;
    time_t timet;

    clock = EGG_CLOCK_FACE (data);
    priv = EGG_CLOCK_FACE_GET_PRIVATE (clock);

    /* update the time */
    time (&timet);
    localtime_r (&timet, &priv->time);

    egg_clock_face_redraw_canvas (clock);

    return TRUE; /* keep running this event */
}

GtkWidget *
egg_clock_face_new (void)
{
    return g_object_new (EGG_TYPE_CLOCK_FACE, NULL);
}
