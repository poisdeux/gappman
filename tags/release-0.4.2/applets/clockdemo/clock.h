/**
 * clock.h
 *
 * A GTK+ widget that implements a clock face
 *
 * (c) 2005, Davyd Madeley
 *
 * Authors:
 *   Davyd Madeley  <davyd@madeley.id.au>
 */

#ifndef __EGG_CLOCK_FACE_H__
#define __EGG_CLOCK_FACE_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS
#define EGG_TYPE_CLOCK_FACE		(egg_clock_face_get_type ())
#define EGG_CLOCK_FACE(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), EGG_TYPE_CLOCK_FACE, EggClockFace))
#define EGG_CLOCK_FACE_CLASS(obj)	(G_TYPE_CHECK_CLASS_CAST ((obj), EGG_CLOCK_FACE, EggClockFaceClass))
#define EGG_IS_CLOCK_FACE(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), EGG_TYPE_CLOCK_FACE))
#define EGG_IS_CLOCK_FACE_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE ((obj), EGG_TYPE_CLOCK_FACE))
#define EGG_CLOCK_FACE_GET_CLASS	(G_TYPE_INSTANCE_GET_CLASS ((obj), EGG_TYPE_CLOCK_FACE, EggClockFaceClass))
typedef struct _EggClockFace EggClockFace;
typedef struct _EggClockFaceClass EggClockFaceClass;

struct _EggClockFace
{
	GtkDrawingArea parent;

	/* < private > */
};

struct _EggClockFaceClass
{
	GtkDrawingAreaClass parent_class;

	void (*time_changed) (EggClockFace * clock, int hours, int minutes);
};

GtkWidget *egg_clock_face_new(void);

G_END_DECLS
#endif
