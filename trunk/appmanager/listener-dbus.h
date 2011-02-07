#ifndef __LISTENER_DBUS_H__
#define __LISTENER_DBUS_H__
#include <glib.h>
/*
 * Type macros.
 */
#define GM_TYPE_APPMANAGER                  (gm_appmanager_get_type ()) ///< always returns the gm_appmanager object type
#define GM_APPMANAGER(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GM_TYPE_APPMANAGER, GmAppmanager)) ///< Used to cast an object to type gm_appman
#define GM_IS_APPMANAGER(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GM_TYPE_APPMANAGER)) ///< Checks whether obj is of type gm_appman
#define GM_APPMANAGER_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), GM_TYPE_APPMANAGER, GmAppmanagerClass)) ///< Used to cast klass to class gm_appman
#define GM_IS_APPMANAGER_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), GM_TYPE_APPMANAGER)) ///< Checks whether klass is of class gm_appman
#define GM_APPMANAGER_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), GM_TYPE_APPMANAGER, GmAppmanagerClass)) ///< Returns the gm_appman class of obj

typedef struct _GmAppmanager        GmAppmanager; ///< Typedef to make switching to different struct easier
typedef struct _GmAppmanagerClass   GmAppmanagerClass; ///< Typedef to make switching to different struct easier

/**
* \struct _GmAppmanager
* GObjectClass struct holding any instances of the gm_appman object.
*/
struct _GmAppmanager
{
  GObject parent_instance; ///< identify this struct as the parent object

  /* instance members */
};

/**
* \struct _GmAppmanagerClass
* GObjectClass struct holding a reference to the parent class
* and any class members (vars and methods)
*/
struct _GmAppmanagerClass
{
  GObjectClass parent_class; ///< identify this struct as the parent class

  /* class members */
};

/**
* \brief used by GM_TYPE_APPMANAGER which should always return the GType
* of the object type.
* \return GType of object GM_APPMANAGER
*/
GType gm_appmanager_get_type ();

//Functions supporting the introspection objects
static gboolean send_fontsize(GmAppmanager *obj, gint *fontsize, GError **error);
static gboolean send_proceslist(GmAppmanager *obj, gchar ***proceslist, GError **error);
static gboolean update_resolution(GmAppmanager *obj, gchar* name, gint width, gint height, GError **error);

//This needs to be included after above function prototype
#include <gm_listener_glue.h>

gboolean listener_dbus_start_session(GtkWidget *window);

#endif /* __LISTENER_DBUS_H__ */
