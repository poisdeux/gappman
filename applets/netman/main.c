/**
 * \file applets/netman/main.c
 * \brief applet showing network status and allows users to analyse and restart
 *        the network.
 *
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 *
 */
#include <gtk/gtk.h>
#include <gmodule.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dbus/dbus-glib.h>
#include <string.h>
#include <gm_layout.h>
#include "parseconf.h"

static GtkWidget *main_button = NULL;
static int main_button_width = 50;
static int main_button_height = 50;
static const char *conffile = SYSCONFDIR "/netman.xml";
static gboolean RUNNING;
static GMutex *check_status_mutex;
static GtkImage *image_unavail;

static void get_lock()
{
	while (g_mutex_trylock(check_status_mutex) == FALSE)
	{
		sleep(1);
	}
}

static void release_lock()
{
	g_mutex_unlock(check_status_mutex);
}

static void destroy_widget(GtkWidget * dummy, GdkEvent * event,
						   GtkWidget * widget)
{
	if (gm_layout_check_key(event))
	{
		gtk_widget_destroy(widget);
	}
}

static void check_status(GPid pid, gint status, nm_elements * elt)
{
	if (WIFEXITED(status))
	{
		// program exited normally
		elt->running = FALSE;
		elt->status = WEXITSTATUS(status);
	}
	else if (WIFSIGNALED(status))
	{
		// program did not exit normally
		elt->running = FALSE;
		elt->status = -1;
	}

	if (g_source_remove(elt->g_source_tag))
	{
		g_spawn_close_pid(pid);
	}

}

static gint exec_program(nm_elements * elt)
{
	GError *error;
	int i;
	char **args;
	GPid childpid;

	error = NULL;

	// We keep the conversion to execv format in elt->argv
	// to prevent converting it each time we exec the program
	if (elt->argv == NULL)
	{
		args = (char **)malloc((elt->numArguments + 2) * sizeof(char *));
		args[0] = (char *)elt->exec;
		for (i = 0; i < elt->numArguments; i++)
		{
			args[i + 1] = elt->args[i];
		}
		args[i + 1] = NULL;
		elt->argv = args;
	}

	if (!g_spawn_async(NULL, elt->argv, NULL,
					   G_SPAWN_DO_NOT_REAP_CHILD | G_SPAWN_STDOUT_TO_DEV_NULL
					   | G_SPAWN_STDERR_TO_DEV_NULL,
					   NULL, NULL, &childpid, &error))
	{
		g_warning("Error executing program: %s", error->message);
		g_error_free(error);
		error = NULL;
		return FALSE;
	}

	elt->pid = childpid;
	elt->running = TRUE;

	elt->g_source_tag =
		g_child_watch_add(childpid, (GChildWatchFunc) check_status, elt);

	return TRUE;
}

static void exec_program_by_gtk_callback(GtkWidget * widget, GdkEvent * event,
										 nm_elements * elt)
{
	if (gm_layout_check_key(event) == FALSE)
	{
		return;
	}
	exec_program(elt);
}

static void perform_action(GtkWidget * widget, GdkEvent * event,
						   nm_elements * elt)
{
	if (gm_layout_check_key(event) == FALSE)
	{
		return;
	}

	if (elt->running != TRUE)
	{
		exec_program(elt);
	}
	else
	{
		gm_layout_show_question_dialog
			("Action already started.\nDo you want to start it anyway?",
			 "Start action", exec_program_by_gtk_callback, elt, "Cancel", NULL,
			 NULL, NULL);
	}
}



static void update_button()
{
  ///< \todo Add a check to see if status has changed to prevent button from being updated every loop.

	nm_elements *elts;

	elts = nm_get_stati();

	while (elts != NULL)
	{
		if ((elts->status == -1) || (elts->status != elts->success))
		{
			gdk_threads_enter();
			gtk_button_set_image(GTK_BUTTON(main_button),
								 GTK_WIDGET(elts->image_fail));
			gdk_threads_leave();

			// Network will only succeed if all checks succeed. So we can
			// stop if one of the checks fails
			return;
		}
		elts = elts->next;
	}

	elts = nm_get_stati();
	gdk_threads_enter();
	gtk_button_set_image(GTK_BUTTON(main_button), GTK_WIDGET(elts->image_success));
	gdk_threads_leave();
}

static GtkWidget* display_status_overview()
{
	nm_elements *stati = NULL;
	gm_menu *menu = NULL;
	gm_menu_element *menu_elt = NULL;	
	GtkWidget *hbox = NULL;
	GtkWidget *label = NULL;
	GdkPixbuf *pixbuf = NULL;
	GtkWidget *stock_image = NULL;

	menu = gm_menu_create();

//	gm_layout_get_window_geometry();
//	gm_menu_set_width();

	stati = nm_get_stati();
	while (stati != NULL)
	{
		hbox = gtk_hbox_new(FALSE, 0);
		label = gm_layout_create_label((gchar*) stati->name);
		gtk_container_add(GTK_CONTAINER(hbox), label);
		gtk_widget_show(label);

		if (stati->status == stati->success)
		{
			pixbuf =
				gtk_widget_render_icon(label, GTK_STOCK_OK,
									   GTK_ICON_SIZE_BUTTON, NULL);
		}
		else
		{
			pixbuf =
				gtk_widget_render_icon(label, GTK_STOCK_NO,
									   GTK_ICON_SIZE_BUTTON, NULL);
		}
		stock_image = gtk_image_new_from_pixbuf(pixbuf);
		gtk_container_add(GTK_CONTAINER(hbox), stock_image);
		gtk_widget_show(stock_image);

		menu_elt = gm_menu_element_create();
		gm_menu_element_set_widget(hbox, menu_elt);
		gm_menu_add_menu_element(menu_elt, menu);

		stati = stati->next;
	}

	gm_layout_calculate_sizes(menu);
	return gm_layout_create_menu(menu);
}

static void show_menu(GtkWidget * dummy, GdkEvent * event,
						   GtkWidget * widget)
{
	nm_elements *actions;
	GtkWidget *vbox;
	GtkWidget *status_overview;
	GtkWidget *button;
	GtkWidget *menuwin;
	GtkWidget *label;
	gchar *markup;

  if ( ! gm_layout_check_key(event))
		return;

	menuwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(menuwin), GTK_WIN_POS_CENTER);
	gtk_window_set_decorated(GTK_WINDOW(menuwin), FALSE);
	gtk_widget_grab_focus(menuwin);
	vbox = gtk_vbox_new(FALSE, 10);

	status_overview = display_status_overview();
	gtk_container_add(GTK_CONTAINER(vbox), status_overview);
	gtk_widget_show(status_overview);

	actions = nm_get_actions();
	while (actions != NULL)
	{
		label = gm_layout_create_label((gchar*) actions->name);
		button = gm_layout_create_empty_button(perform_action, actions);
		gtk_container_add(GTK_CONTAINER(button), label);
		gtk_widget_show(label);
		gtk_container_add(GTK_CONTAINER(vbox), button);
		gtk_widget_show(button);

		actions = actions->next;
	}

	button = gm_layout_create_label_button("Cancel", destroy_widget, menuwin);
	gtk_container_add(GTK_CONTAINER(vbox), button);
	gtk_widget_show(button);
	gtk_widget_show(vbox);
	gtk_container_add(GTK_CONTAINER(menuwin), vbox);
	gtk_widget_show(menuwin);
}

static GmReturnCode load_images()
{
	nm_elements *stati;

	stati = nm_get_stati();
	while (stati != NULL)
	{

		stati->image_success =
			GTK_IMAGE(gm_layout_load_image
					  ("gm_netman_success", (char *)stati->logosuccess,
					   (char *)nm_get_cache_location(), (char *)stati->name,
					   main_button_width, main_button_height));
		gtk_widget_show(GTK_WIDGET(stati->image_success));
		g_object_ref(stati->image_success);

		stati->image_fail =
			GTK_IMAGE(gm_layout_load_image
					  ("gm_netman_fail", (char *)stati->logofail,
					   (char *)nm_get_cache_location(), (char *)stati->name,
					   main_button_width, main_button_height));
		gtk_widget_show(GTK_WIDGET(stati->image_fail));
		g_object_ref(stati->image_fail);

		stati = stati->next;
	}

	if (nm_get_filename_logounavail() != NULL)
	{
		image_unavail =
			GTK_IMAGE(gm_layout_load_image
					  ("gm_netman_unavail",
					   (char *)nm_get_filename_logounavail(),
					   (char *)nm_get_cache_location(), "netman",
					   main_button_width, main_button_height));
		gtk_widget_show(GTK_WIDGET(image_unavail));
		g_object_ref(image_unavail);

		// We start off in unavail mode
		gtk_container_add(GTK_CONTAINER(main_button),
						  GTK_WIDGET(image_unavail));
	}
	else
	{
		stati = nm_get_stati();
		gtk_container_add(GTK_CONTAINER(main_button),
						  GTK_WIDGET(stati->image_fail));
	}

	return GM_SUCCESS;
}

/**
* \brief Initializes the module. Loads the configuration and starts the applet in failed mode.
* \return int 
*  - GM_SUCCESS if initialization was succesful. 
*  - GM_COULD_NOT_LOAD_FILE if the configuration file could not be loaded.
*/
G_MODULE_EXPORT GmReturnCode gm_module_init()
{
	if (nm_load_conf(conffile) != GM_SUCCESS)
		return GM_COULD_NOT_LOAD_FILE;

	main_button = gm_layout_create_empty_button(G_CALLBACK(show_menu), NULL);

	load_images();

	gtk_widget_show(main_button);

	check_status_mutex = g_mutex_new();

	return GM_SUCCESS;
}

/**
* \brief Sets the location of the configuration file
*/
G_MODULE_EXPORT void gm_module_set_conffile(const char *filename)
{
	conffile = filename;
}


/**
* \brief Starts the applet checking network status
* Uses global RUNNING to determine if it should keep
* checking the network status. If RUNNING becomes FALSE
* it will stop.
*/
G_MODULE_EXPORT void gm_module_start()
{
	nm_elements *elts;
	int sleep_left = 0;
	if (RUNNING == TRUE)
	{
		g_warning("gm_netman applet already started");
		return;
	}

	RUNNING = TRUE;

	while (RUNNING)
	{
		get_lock();
		elts = nm_get_stati();
		while ((elts != NULL))
		{
			if (elts->running != TRUE)
			{
				exec_program(elts);
				elts->running = TRUE;
			}
			elts = elts->next;
		}
		update_button();
		release_lock();

		//we try to sleep for 10 seconds
    //if the sleep gets interrupted by
    //a signal we continue to sleep untill
    //all 10 seconds have passed
		sleep_left = sleep(10);
		while (sleep_left != 0)
		{
			sleep_left = sleep(sleep_left);
		}
	}
}

/**
* \brief Stops the applet
* Sets RUNNING to FALSE and frees the stati 
* list of status structs.
*/
G_MODULE_EXPORT int gm_module_stop()
{
	nm_elements *elts, *tmp;

	if (RUNNING == FALSE)
	{
		g_warning("gm_netman applet not running");
		return;
	}

	RUNNING = FALSE;

	// prevents freeing elements while
	// we are still in a run checking
	// some status
	get_lock();

	g_object_unref(image_unavail);

	elts = nm_get_stati();
	tmp = elts;
	while (tmp != NULL)
	{
		g_object_unref(tmp->image_success);
		g_object_unref(tmp->image_fail);
		tmp = tmp->next;
	}
	nm_free_elements(elts);

	release_lock();

	return GM_SUCCESS;
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
	return main_button;
}
