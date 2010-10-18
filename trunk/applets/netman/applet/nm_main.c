/**
 * \file nm_main.c
 * \brief applet showing network status and allows users to analyse and restart the network.
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
#include <unistd.h>
#include <sys/wait.h>
#include <gm_layout.h>
#include <gm_generic.h>
#include <dbus/dbus-glib.h>
#include "nm_parseconf.h"

static GtkButton *main_button = NULL;
static int main_button_width = 50;
static int main_button_height = 50;
static const char* conffile = "/etc/gappman/netman.xml";
static gboolean KEEP_RUNNING = FALSE;
static DBusGProxy *remote_object;
static DBusGConnection *bus;

static gboolean dbus_connect()
{
	GError *error = NULL;

  bus = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
  if (!bus)
  {
    g_warning ("Couldn't connect to session bus: %s\n", error->message);
		return FALSE;
  }

  remote_object = dbus_g_proxy_new_for_name (bus,
               "gappman.netman",
               "/GmNetmand",
               "gappman.netman.NetmanInterface");

	return TRUE;
}

static gboolean check_status(nm_elements *check)
{
  GError *error = NULL;
	gchar* args[] = { "-c", "1", "google.com", NULL }; 
	gint status;

  if (!dbus_g_proxy_call (remote_object, "RunCommand", &error,
        G_TYPE_STRING, "ping", G_TYPE_STRV, args, G_TYPE_INVALID,
        G_TYPE_INT, &status, G_TYPE_INVALID))
	{
		g_warning ("Failed to complete RunCommand: %p %s", remote_object, error->message);
		return FALSE;
	}

	return TRUE;
}

static void destroy_widget(GtkWidget* dummy, GdkEvent *event, GtkWidget* widget)
{
		//Only start program  if spacebar or mousebutton is pressed
    if ( ((GdkEventKey*)event)->keyval == 32 || ((GdkEventButton*)event)->button == 1)
    {
    	gtk_widget_destroy(widget);
		}
}

static void update_button()
{
	nm_elements* checks;
	int status_changed_to_succes = 0;

	checks = nm_get_stati();
	
	while(checks != NULL)
	{
		//we only need to do something if the status changed
		if(checks->prev_status != checks->status)
		{
			if(checks->status != checks->success)
			{
				gtk_button_set_image(main_button, GTK_WIDGET(checks->image_fail));

				// Network will only succeed if all checks succeed. So we can stop
				// if one of the checks fails
				return;
			}
			else
			{
				status_changed_to_succes = 1;
			}
		}
		checks = checks->next;
	}	
	//we'll only get this far if none of the checks failed or did not change
	//since the previous check.
	if( status_changed_to_succes )
	{
		gtk_button_set_image(main_button, GTK_WIDGET(checks->image_success));
	}
}

static void show_menu()
{
    nm_elements* actions;
    GtkWidget* vbox;
    GtkWidget* table;
    GtkWidget* button;
    GtkWidget* menuwin;
    GtkWidget* label;
    GdkPixbuf *pixbuf;
    GtkWidget *stock_image;
    gchar* markup;
    nm_elements* stati;
		int elt_nr = 0;

    menuwin = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW (menuwin), GTK_WIN_POS_CENTER);
    gtk_window_set_decorated (GTK_WINDOW (menuwin), FALSE);
		gtk_widget_grab_focus(menuwin);

		gtk_widget_set_name(menuwin, "gm_applet");
    vbox = gtk_vbox_new (FALSE, 10);

    stati = nm_get_stati();

    actions = nm_get_actions();

	table = gtk_table_new(2, *stati->amount_of_elements, TRUE);
	while (stati != NULL)
	{
    markup = g_markup_printf_escaped ("<span size=\"%d\">%s</span>", gm_get_fontsize(), stati->name);
		label = gtk_label_new("");
    gtk_label_set_markup (GTK_LABEL (label), markup);
    g_free (markup);
		gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, elt_nr, elt_nr+1);
    gtk_widget_show(label);

		if(stati->status == stati->success)
		{
			pixbuf = gtk_widget_render_icon(label, GTK_STOCK_OK, GTK_ICON_SIZE_BUTTON, NULL);
		}
		else
		{
			pixbuf = gtk_widget_render_icon(label, GTK_STOCK_NO, GTK_ICON_SIZE_BUTTON, NULL);
		}
	  stock_image = gtk_image_new_from_pixbuf(pixbuf);
		gtk_table_attach_defaults (GTK_TABLE (table), stock_image, 1, 2, elt_nr, elt_nr+1);

		gtk_widget_show (stock_image);

    stati = stati->next;
		elt_nr++;
	}
	gtk_container_add(GTK_CONTAINER (vbox), table);	
	gtk_widget_show(table);

    while (actions != NULL)
    {
        label = gtk_label_new("");
        markup = g_markup_printf_escaped ("<span size=\"%d\">%s</span>", gm_get_fontsize(), actions->name);
        gtk_label_set_markup (GTK_LABEL (label), markup);
        g_free (markup);
        button = gm_create_empty_button(check_status, actions);
				gtk_container_add(GTK_CONTAINER(button), label);
        gtk_widget_show(label);
        gtk_container_add(GTK_CONTAINER(vbox), button);
        gtk_widget_show(button);

        actions = actions->next;
    }

    button = gm_create_label_button("Cancel", destroy_widget, menuwin);
    gtk_container_add(GTK_CONTAINER(vbox), button);
    gtk_widget_show(button);
    gtk_widget_show(vbox);
    gtk_container_add(GTK_CONTAINER(menuwin), vbox);
    gtk_widget_show(menuwin);
}

/**
* \brief Initializes the module. Loads the configuration and starts the applet in failed mode.
* \return int 
*  - GM_SUCCES if initialization was succesful. 
*  - GM_COULD_NOT_LOAD_FILE if the configuration file could not be loaded.
*/
G_MODULE_EXPORT int gm_module_init()
{
    nm_elements* stati;
	
  	g_type_init ();

    if(nm_load_conf(conffile) != 0)
			return GM_COULD_NOT_LOAD_FILE;

    stati = nm_get_stati();

    main_button = GTK_BUTTON(gtk_button_new());

    g_signal_connect (G_OBJECT (main_button),
                      "clicked",
                      G_CALLBACK (show_menu),
                      NULL);
	while (stati != NULL)
	{
    	stati->image_success = GTK_IMAGE(gm_load_image("gm_netman_success", (char*) stati->logosuccess, (char*) nm_get_cache_location(), (char*) stati->name, main_button_width, main_button_height));
    	gtk_widget_show(GTK_WIDGET(stati->image_success));
		g_object_ref(stati->image_success);

    	stati->image_fail = GTK_IMAGE(gm_load_image("gm_netman_fail", (const char*) stati->logofail, nm_get_cache_location(), (char*) stati->name, main_button_width, main_button_height));
    	gtk_widget_show(GTK_WIDGET(stati->image_fail));
		g_object_ref(stati->image_fail);
		
		stati = stati->next;
	}
    stati = nm_get_stati();
    //We start off in fail mode
    gtk_container_add(GTK_CONTAINER(main_button), GTK_WIDGET(stati->image_fail));
    gtk_widget_show(GTK_WIDGET(main_button));

		if ( dbus_connect() == FALSE )
			return GM_FAIL;

    return GM_SUCCES;
}

/**
* \brief Supposed to be used by gappman to make the module aware of its configuration file
* Not really used so will be deprecated pretty soon.
*/
G_MODULE_EXPORT void gm_module_set_conffile(const char* filename)
{
    conffile = filename;
}

/**
* \brief Starts the applet checking network status
* Uses global KEEP_RUNNING to determine if it should keep
* checking the network status. If KEEP_RUNNING becomes FALSE
* it will stop.
*/
G_MODULE_EXPORT void gm_module_start()
{
	nm_elements *checks;
	int failed_checks = 0;

	KEEP_RUNNING = TRUE;
	while (KEEP_RUNNING)
	{
		checks = nm_get_stati();

		if( *checks->amount_of_elements * 10 < failed_checks )
		{
			g_warning("Exiting. gm_netmand is not running.");
			// NOTE: possible race condition when both gm_module_start 
			// and appmanager stop the module.
			// add new function gm_module_running() to let appmanager check
			// periodically if the module is still active.
			//gm_module_stop();
			return;
		}

		while(checks != NULL)
		{
			if ( check_status(checks) == FALSE )
			{
				failed_checks++;
				sleep(1);
			}
			else
			{
				failed_checks = 0;
			}
			checks = checks->next;
		}
//		update_button();
		sleep(2);
	}
}

/**
* \brief Stops the applet
* Sets KEEP_RUNNING to FALSE and frees the stati 
* list of status structs.
*/
G_MODULE_EXPORT int gm_module_stop()
{
  nm_elements *checks, *tmp;
  checks = nm_get_stati();
	KEEP_RUNNING = FALSE;	

  g_object_unref (G_OBJECT (remote_object));

	tmp = checks;
	while(tmp != NULL)
	{
		g_object_unref(tmp->image_success);
		g_object_unref(tmp->image_fail);
		tmp = tmp->next;
	}
	nm_free_elements(checks);
  return GM_SUCCES;
}

/**
* \brief Sets the icon size as determined by gappman
* \param width width of the icon
* \param height height of the icon
*/
G_MODULE_EXPORT void gm_module_set_icon_size(int width, int height)
{
    main_button_width = width;
    main_button_height = height;
}

/**
* \brief returns the button that gappman should add to the applet-bar
* \return GtkWidget
*/
G_MODULE_EXPORT GtkWidget *gm_module_get_widget()
{
    return GTK_WIDGET(main_button);
}

