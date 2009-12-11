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
#include <gtk/gtk.h>
#include <gmodule.h>
#include <stdio.h>
#include "gm_parseconf.h"

static GtkWidget *button = NULL;
static int button_width = 50;
static int button_height = 50;
static const char* conffile = "./widgets/netman/xml-config-files/netman.xml";

G_MODULE_EXPORT int gm_module_init()
{
	printf("Woohoo netman speaking!\n");		
	button = gtk_button_new_with_label("test");
	nm_load_conf(conffile);		
	return 0;
}

G_MODULE_EXPORT void gm_module_set_conffile(const char* filename)
{
	conffile = filename;
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
	button_width = width;
	button_height = height;
}

G_MODULE_EXPORT GtkWidget *gm_module_get_widget()
{
	return button;
}

