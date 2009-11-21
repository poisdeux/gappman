/***
 * \file gm_netman.c_ 
 *
 * 
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 */

#include <gm_netman.h>
#include <gmodule.h>

GtkWidget *button = NULL;

G_MODULE_EXPORT gboolean
int gm_module_init()
{
		
	return 0;
}

G_MODULE_EXPORT gboolean
int gm_module_start()
{
	return 0;
}

G_MODULE_EXPORT gboolean
int gm_module_stop()
{
	return 0;
}

G_MODULE_EXPORT gboolean
void gm_module_set_icon_size(int width, int height)
{

}

G_MODULE_EXPORT gboolean
GtkWidget *gm_module_get_widget()
{
	return button;
}



GType gm_netman_get_type()
{
	static GType gm_netman_type = 0;

	if (!gm_netman_type)
	{
		static const GTypeInfo gm_netman_info =
		{
			sizeof(gm_netman_class),
			NULL, NULL,
			(GClassInitFunc) gm_netman_class_init,
			NULL, NULL,
			sizeof(gm_netman_struct),
			0,
			(GInstanceInitFunc) gm_netman_init,
		};

		gm_netman_type = g_type_register_static (GTK_TYPE_WIDGET, "gm_netman", &gm_netman_info, 0);
	}
	return gm_netman_type;
}
