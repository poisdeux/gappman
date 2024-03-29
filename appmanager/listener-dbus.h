/**
 * \file listener-dbus.h
 *
 *
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 */

#ifndef __LISTENER_DBUS_H__
#define __LISTENER_DBUS_H__
#include <glib.h>
/* 
 * Type macros.
 */

/**
 * \brief always returns the gm_appmanager object type
 */
#define GM_TYPE_APPMANAGER                  (gm_appmanager_get_type ())

/**
 * \brief Used to cast an object to type GmAppmanager
 */
#define GM_APPMANAGER(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GM_TYPE_APPMANAGER, GmAppmanager))

/**
 * \brief Checks whether obj is of type GmAppmanager
 */
#define GM_IS_APPMANAGER(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GM_TYPE_APPMANAGER))

/**
 * \brief Used to cast klass to class GmAppmanager
 */
#define GM_APPMANAGER_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), GM_TYPE_APPMANAGER, GmAppmanagerClass)) 

/**
 * \brief Checks whether klass isof class GmAppmanager
 */
#define GM_IS_APPMANAGER_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), GM_TYPE_APPMANAGER))

/**
 * \brief Returns the GmAppmanager class of obj
 */
#define GM_APPMANAGER_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), GM_TYPE_APPMANAGER, GmAppmanagerClass))

/**
 * \typedef GmAppmanager
 * \brief making code more readable....
 */
typedef struct _GmAppmanager GmAppmanager;

/**
 * \typedef GmAppmanagerClass
 * \brief making code more readable....
 */
typedef struct _GmAppmanagerClass GmAppmanagerClass;

/**
* \struct _GmAppmanager
* GObjectClass struct holding any instances of the gm_appman object.
*/
struct _GmAppmanager
{
	GObject parent_instance;	///< identify this struct as the parent
								// object

	/* instance members */
};

/**
* \struct _GmAppmanagerClass
* GObjectClass struct holding a reference to the parent class
* and any class members (vars and methods)
*/
struct _GmAppmanagerClass
{
	GObjectClass parent_class;	///< identify this struct as the parent class

	/* class members */
};

/**
* \brief used by GM_TYPE_APPMANAGER which should always return the GType
* of the object type.
* \return GType of object GM_APPMANAGER
*/
GType gm_appmanager_get_type();

/// Functions supporting the introspection objects
/**
* \brief gets current fontsize from gappman and returns it by reference in fontsize
*/
gboolean send_fontsize(GmAppmanager * obj, gint * fontsize,
							  GError ** error);
/**
* \brief gets current fontsize from gappman and returns it by reference in fontsize
*/
gboolean send_window_geometry(GmAppmanager * obj, gint * width, gint *height,
							  GError ** error);
/**
* \brief gets current fontsize from gappman and returns it by reference in fontsize
*/
gboolean send_confpath(GmAppmanager * obj, gchar ** path,
							  GError ** error);
/**
* \brief gets current fontsize from gappman and returns it by reference in fontsize
*/
gboolean send_proceslist(GmAppmanager * obj, gchar *** proceslist,
								GError ** error);
/**
* \brief gets current fontsize from gappman and returns it by reference in fontsize
*/
gboolean update_resolution(GmAppmanager * obj, gchar * name, gint width,
								  gint height, GError ** error);


/**
 * \brief registers gappman to the D-Bus session bus
 * \return always returns true. This might change in the future.
 */
gboolean listener_dbus_start_session();

#endif /* __LISTENER_DBUS_H__ */
