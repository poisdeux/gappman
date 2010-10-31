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
#include <string.h>
#include "nm_parseconf.h"

static GtkButton *main_button = NULL;
static int main_button_width = 50;
static int main_button_height = 50;
static const char* conffile = "/etc/gappman/netman.xml";
static gboolean KEEP_RUNNING;
static GMutex *check_status_mutex;

static DBusGProxyCallNotify collect_status(DBusGProxy *proxy, DBusGProxyCall* proxy_call, nm_elements *check)
{
	GError *error = NULL;
	gint status;

	if( ! dbus_g_proxy_end_call(proxy, proxy_call, &error,
      	G_TYPE_INT, &status, G_TYPE_INVALID) )
	{
		g_warning("Error retrieving status for command %s: %s", check->exec, error->message);
		g_error_free(error);

		check->prev_status = check->status;
		check->status = -1;
	}
	else
	{
		check->prev_status = check->status;
		check->status = status;
	}
	g_message("Got status: %d", check->status);
	check->checking = FALSE;
}

static gboolean check_status(nm_elements *check)
{
  GError *error = NULL;
  gchar* args[] = { "-c", "1", "google.com", NULL };
	gint status;
	DBusGProxyCall* proxy_call;
  DBusGConnection *bus;
  DBusGProxy *proxy;

	bus = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
  if (bus == NULL)
  {
    g_warning ("Couldn't connect to session bus: %s\n", error->message);
    g_error_free(error);
    error = NULL;
    return FALSE;
  }

  proxy = dbus_g_proxy_new_for_name (bus,
               "gappman.netman",
               "/GmNetmand",
               "gappman.netman.NetmanInterface");

  if(proxy == NULL)
  {
    g_warning("Could not get dbus object for gappman.netman.NetmanInterface");
    dbus_g_connection_unref(bus);
		return FALSE;
  }


	proxy_call = dbus_g_proxy_begin_call(proxy, 
			"RunCommand", (DBusGProxyCallNotify) collect_status, check,	NULL, 
			G_TYPE_STRING, check->exec, G_TYPE_STRV, check->args, G_TYPE_INVALID);

	if (proxy_call == NULL)
  {
  	g_warning ("Failed to call RunCommand: %s", error->message);
   	g_error_free(error);
    error = NULL;
		return FALSE;
  }

	check->checking = TRUE;

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
			if( checks->status == -1 )
			{
				gtk_button_set_image(main_button, GTK_WIDGET(checks->image_unavail));
			}
			else if(checks->status != checks->success)
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
		checks = nm_get_stati();
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
        button = gm_create_empty_button(NULL, NULL);
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
    stati->image_unavail = GTK_IMAGE(gm_load_image("gm_netman_unavail", (char*) stati->logounavail, (char*) nm_get_cache_location(), (char*) stati->name, main_button_width, main_button_height));
    gtk_widget_show(GTK_WIDGET(stati->image_unavail));
		g_object_ref(stati->image_unavail);

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

	check_status_mutex = g_mutex_new();

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


	if ( KEEP_RUNNING == TRUE )
	{
		g_warning("gm_netman applet already started");
		return;
	}

	KEEP_RUNNING = TRUE;

	while(KEEP_RUNNING)
	{
		checks = nm_get_stati();
		while((checks != NULL) && KEEP_RUNNING)
		{
			//lock required to prevent gm_module_stop
			//from kicking in when we are just about
			//to perform a check
			if( g_mutex_trylock(check_status_mutex) )
			{
				if(!checks->checking)
				{
					gdk_threads_enter();
					check_status(checks);
					gdk_threads_leave();
				}
				checks = checks->next;
				g_mutex_unlock(check_status_mutex);
			}
		}

    if( g_mutex_trylock(check_status_mutex) )
    {
			gdk_threads_enter();
			update_button();
			gdk_threads_leave();
			g_mutex_unlock(check_status_mutex);
		}

		sleep(1);
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
	
	if ( KEEP_RUNNING == FALSE )
	{
		g_warning("gm_netman applet not running");
		return;
	}

	KEEP_RUNNING = FALSE;

	//prevents freeing elements while
	//we are still in a run checking
  //some status
	while(! g_mutex_trylock(check_status_mutex))
	{
		sleep(1);
	}

  checks = nm_get_stati();

	tmp = checks;
	while(tmp != NULL)
	{
		g_object_unref(tmp->image_unavail);
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

