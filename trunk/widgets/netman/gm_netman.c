/***
 * \file main.c
 *
 * 
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 */

#include <gm_netman.h>

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
