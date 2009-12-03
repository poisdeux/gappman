/***
 * \file gm_netman.h
 *
 * 
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 */

#ifndef __GM_NETMAN_H__
#define __GM_NETMAN_H__

#include <glib.h>
#include <gdk/gdk.h>
#include <gtk/gtkwidget.h>

G_BEGIN_DECLS

#define GM_NETMAN_TYPE (gm_netman_get_type())
#define GM_NETMAN(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GM_NETMAN_TYPE, gm_netman_struct))
#define GM_NETMAN_CLASS(gmclass) (G_TYPE_CHECK_CLASS_CAST ((gmclass), GM_NETMAN_TYPE, gm_netman_class))
#define IS_GM_NETMAN(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GM_NETMAN_TYPE))
#define IS_MY_MARGUEE_CLASS(gmclass) (G_TYPE_CHECK_CLASS_TYPE ((gmclass), GM_NETMAN_TYPE))

typedef struct _gm_netman_struct gm_netman_struct;
typedef struct _gm_netman_class gm_netman_class;

struct _gm_netman_struct
{
	GtkWidget widget;
};

struct _gm_netman_class
{
	GtkWidgetClass parent_class;
};

GType gm_netman_get_type(void) G_GNUC_CONST;

GtkWidget* gm_netman_new(void);

G_END_DECLS

#endif
