#ifndef __GM_NETMAND_H__
#define __GM_NETMAND_H__
#include <glib.h>

//Functions supporting the introspection objects
static int gm_netmand_run_command(gchar* command, gchar** args);

#include <gm_netmand_glue.h>

/*
 * Type macros.
 */
#define GM_TYPE_NETMAND                  (gm_netmand_get_type ())
#define GM_NETMAND(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GM_TYPE_NETMAND, GmNetmand))
#define GM_IS_NETMAND(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GM_TYPE_NETMAND))
#define GM_NETMAND_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), GM_TYPE_NETMAND, GmNetmandClass))
#define GM_IS_NETMAND_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), GM_TYPE_NETMAND))
#define GM_NETMAND_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), GM_TYPE_NETMAND, GmNetmandClass))

typedef struct _GmNetmand        GmNetmand;
typedef struct _GmNetmandClass   GmNetmandClass;

/**
* \struct _GmNetmand
* GObjectClass struct holding any instances of the gm_netmand object.
*/
struct _GmNetmand
{
  GObject parent_instance; ///< identify this struct as the parent object

  /* instance members */
};

/**
* \struct _GmNetmandClass
* GObjectClass struct holding a reference to the parent class
* and any class members (vars and methods) 
*/
struct _GmNetmandClass
{
  GObjectClass parent_class; ///< identify this struct as the parent class

  /* class members */
};

/**
* \brief used by GM_TYPE_NETMAND which should always return the GType
* of the object type.
* \return GType of object GM_NETMAND
*/ 
GType gm_netmand_get_type ();

#endif /* __GM_NETMAND_H__ */
