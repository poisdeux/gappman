/**
 * \file gm_netmand.h
 * Daemon that checks network status on behalf of the gm_netman applet 
 * Used D-Bus for communication with the applet
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 */

#ifndef __GM_NETMAND_H__
#define __GM_NETMAND_H__
#include <glib.h>



/*
 * Type macros.
 */
#define GM_TYPE_NETMAND                  (gm_netmand_get_type ()) ///< always returns the gm_netmand object type
#define GM_NETMAND(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GM_TYPE_NETMAND, GmNetmand)) ///< Used to cast an object to type gm_netmand
#define GM_IS_NETMAND(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GM_TYPE_NETMAND)) ///< Checks whether obj is of type gm_netmand
#define GM_NETMAND_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), GM_TYPE_NETMAND, GmNetmandClass)) ///< Used to cast klass to class gm_netmand
#define GM_IS_NETMAND_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), GM_TYPE_NETMAND)) ///< Checks whether klass is of class gm_netmand
#define GM_NETMAND_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), GM_TYPE_NETMAND, GmNetmandClass)) ///< Returns the gm_netmand class of obj

typedef struct _GmNetmand        GmNetmand; ///< Typedef to make switching to different struct easier
typedef struct _GmNetmandClass   GmNetmandClass; ///< Typedef to make switching to different struct easier

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

//Functions supporting the introspection objects
static gboolean gm_netmand_run_command(GmNetmand *obj, gchar* command, gchar** args, GError **error);

//This needs to be included after above function prototype
#include <gm_netmand_glue.h>

#endif /* __GM_NETMAND_H__ */
