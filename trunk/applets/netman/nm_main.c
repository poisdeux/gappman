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
#include <unistd.h>
#include <sys/wait.h>
#include <gm_layout.h>
#include "nm_parseconf.h"
//#include "nm_layout.h"

static GtkWidget *main_button = NULL;
static int main_button_width = 50;
static int main_button_height = 50;
static int current_status = -2;
static const char* conffile = "./applets/netman/xml-config-files/netman.xml"; 
static GtkWidget *image_success = NULL;
static GtkWidget *image_fail = NULL;
static char **args;

static gint exec_program(nm_elements* elt)
{
	int status = -1;
  int ret;
  int i;
  __pid_t childpid;
  FILE *fp;

  
  fp = fopen((char *) elt->exec,"r");
  if( fp )
  {
		fclose(fp);

    childpid = fork();
    if ( childpid == 0 )
    {
			/**
  		  Create argument list. First element should be the filename
   		 of the executable and last element needs to be NULL.
   		 see man exec for more details
 		 */
  		args = (char **) malloc((elt->numArguments + 2)* sizeof(char *));
  		args[0] = (char *) elt->exec;
  		for (i = 0; i < elt->numArguments; i++ )
 		 	{
		    args[i+1] = elt->args[i];
 			}
  		args[i+1] = NULL;

      execvp((char *) elt->exec, args);
      _exit(0);
		}

		while( status == -1 )
		{
			waitpid(childpid, &status, WNOHANG); 
			sleep(1);
		} 
	}
	else
	{
		g_warning("Could not open %s\n", elt->exec);
	}
	return status;
}

static int check_status()
{
	int status_fail = 0;
	nm_elements* stati;

	stati = nm_get_stati();

	while(stati != NULL)
	{
		stati->status = exec_program(stati);
		if (stati->status != stati->success)
		{
			status_fail = 1;
		}
		stati = stati->next;
	}
	return status_fail;
}

static void show_status(int status)
{
	if( status == 0 )
	{
		gtk_container_remove(GTK_CONTAINER(main_button), image_fail);
		gtk_container_add(GTK_CONTAINER(main_button), image_success);
		gtk_widget_show(image_success);
	}
	else
	{
		gtk_container_remove(GTK_CONTAINER(main_button), image_success);
		gtk_container_add(GTK_CONTAINER(main_button), image_fail);
		gtk_widget_show(image_fail);
	}
}

static void start_action(GtkWidget* widget, nm_elements* action)
{
	exec_program(action);	
}

static void destroy_widget(GtkWidget* dummy, GtkWidget* widget)
{
	gtk_widget_destroy(widget);
}

static void show_menu()
{
	nm_elements* actions;
	GtkWidget* vbox;
	GtkWidget* button;
	GtkWidget* menuwin;
	GtkWidget* label;
	gchar* markup;

  menuwin = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW (menuwin), GTK_WIN_POS_CENTER);
	gtk_window_set_decorated (GTK_WINDOW (menuwin), FALSE);

  vbox = gtk_vbox_new (FALSE, 10);

	actions = nm_get_actions();

	while(actions != NULL)
	{
		label = gtk_label_new("");
  	markup = g_markup_printf_escaped ("<span size=\"%d\">%s</span>", gm_get_fontsize(), actions->name);
  	gtk_label_set_markup (GTK_LABEL (label), markup);
  	g_free (markup);
  	button = gtk_button_new();
  	gtk_container_add(GTK_CONTAINER(button), label);
  	gtk_widget_show(label);
  	g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (start_action), actions);
  	gtk_container_add(GTK_CONTAINER(vbox), button);
  	gtk_widget_show(button);

		actions = actions->next;
	}

	button = gm_create_cancel_button(destroy_widget, menuwin);
	gtk_container_add(GTK_CONTAINER(vbox), button);
	gtk_widget_show(button);
	gtk_widget_show(vbox);	
  gtk_container_add(GTK_CONTAINER(menuwin), vbox);
	gtk_widget_show(menuwin);
}

G_MODULE_EXPORT int gm_module_init()
{
	nm_elements* stati;

	nm_load_conf(conffile);

	stati = nm_get_stati();

	main_button = gtk_button_new();

	image_fail = gm_load_image((char*) stati->name, (char*) stati->logofail, nm_get_cache_location(), "netman-fail", main_button_width, main_button_height);	
	image_success = gm_load_image((char*) stati->name, (char*) stati->logosuccess, nm_get_cache_location(), "netman-success", main_button_width, main_button_height);	
	gtk_container_add(GTK_CONTAINER(main_button), image_fail);
	gtk_widget_show(image_fail);
	g_signal_connect (G_OBJECT (main_button),
                      "clicked",
                      G_CALLBACK (show_menu),
                      NULL);
	return 0;
}

G_MODULE_EXPORT void gm_module_set_conffile(const char* filename)
{
	conffile = filename;
}

G_MODULE_EXPORT GThreadFunc gm_module_start()
{
	int status = -1;
	int prev_status = -1;
	while(1)
	{
		status = check_status();
		if ( status != prev_status )
		{
			show_status(status);
			prev_status = status;
		}
		sleep(10);
	}
}

G_MODULE_EXPORT int gm_module_stop()
{
	free(args);	
	return 0;
}

G_MODULE_EXPORT void gm_module_set_icon_size(int width, int height)
{
	main_button_width = width;
	main_button_height = height;
}

G_MODULE_EXPORT GtkWidget *gm_module_get_widget()
{
	return main_button;
}

