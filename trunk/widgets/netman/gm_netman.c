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

#include <gmodule.h>

GtkWidget *button = NULL;

G_MODULE_EXPORT int gm_module_init()
{
	printf("Woohoo netman speaking!\n");		
	return 0;
}

G_MODULE_EXPORT int gm_module_start()
{
	return 0;
}

G_MODULE_EXPORT int gm_module_stop()
{
	return 0;
}

G_MODULE_EXPORT void gm_module_set_icon_size(int width, int height)
{

}

G_MODULE_EXPORT GtkWidget *gm_module_get_widget()
{
	return button;
}

