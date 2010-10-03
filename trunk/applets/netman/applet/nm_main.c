/***
 * \file nm_main.c
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
#include <gm_generic.h>
#include "nm_parseconf.h"

static GtkButton *main_button = NULL;
static int main_button_width = 50;
static int main_button_height = 50;
static const char* conffile = "/etc/gappman/netman.xml";
static int KEEP_RUNNING = 0;
static GMutex *gm_module_start_mutex = NULL;

static gint exec_program(nm_elements* elt)
{
    int status = -1;
    int i;
	char **args;
    __pid_t childpid;
    FILE *fp;


    fp = fopen((char *) elt->exec,"r");
    if ( fp )
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
        wait(&status);
    }
    else
    {
        g_warning("Could not open %s\n", elt->exec);
    }
    return WEXITSTATUS(status);
}

static void switch_status(nm_elements* stati)
{
	
    if (stati->status != stati->success)
    {
		gtk_button_set_image(main_button, GTK_WIDGET(stati->image_fail));
    }
	else
	{
		gtk_button_set_image(main_button, GTK_WIDGET(stati->image_success));
	}
}


static void check_status()
{
    int prev_status = 0;
    nm_elements* stati;

    stati = nm_get_stati();

    while (stati != NULL)
    {
			prev_status = stati->status;
    	stati->status = exec_program(stati);
			//Switch only if status is fail or different from previous status
			//i.e. from fail to success.
			g_debug("TEST:%s stati->status=%d   prev_status=%d", stati->name, stati->status, prev_status);
			if((stati->status != prev_status) || (stati->status != stati->success))
			{
				switch_status(stati);
			}
      sleep(2); 
      stati = stati->next;
    }
}

static void start_action(GtkWidget* widget, GdkEvent *event, nm_elements* action)
{
		//Only start program  if spacebar or mousebutton is pressed
    if ( ((GdkEventKey*)event)->keyval == 32 || ((GdkEventButton*)event)->button == 1)
    {
	    exec_program(action);
		}
}

static void destroy_widget(GtkWidget* dummy, GdkEvent *event, GtkWidget* widget)
{
		//Only start program  if spacebar or mousebutton is pressed
    if ( ((GdkEventKey*)event)->keyval == 32 || ((GdkEventButton*)event)->button == 1)
    {
    	gtk_widget_destroy(widget);
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
        button = gm_create_empty_button(start_action, actions);
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

G_MODULE_EXPORT int gm_module_init()
{
    nm_elements* stati;
	
	g_assert(gm_module_start_mutex == NULL);
	gm_module_start_mutex = g_mutex_new();

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

    return GM_SUCCES;
}

G_MODULE_EXPORT void gm_module_set_conffile(const char* filename)
{
    conffile = filename;
}

G_MODULE_EXPORT void gm_module_start()
{
	nm_elements *stati;

	g_mutex_lock(gm_module_start_mutex);
	KEEP_RUNNING = 1;
    while (KEEP_RUNNING)
    {
		stati = nm_get_stati();
		while(stati != NULL)
		{
        	check_status();
			if (stati->status == stati->success)
			{
				stati->status = 10;
			}
			else
			{
				stati->status = stati->success;
			}
			switch_status(stati);
        	//sleep(1);
			stati = stati->next;
		}
    }
	g_mutex_unlock(gm_module_start_mutex);
}

G_MODULE_EXPORT int gm_module_stop()
{
    nm_elements* stati;
    stati = nm_get_stati();
	KEEP_RUNNING = 0;	

	while(stati != NULL)
	{
		g_object_unref(stati->image_success);
		g_object_unref(stati->image_fail);
		stati = stati->next;
	}
	nm_free_elements(stati);
    return GM_SUCCES;
}

G_MODULE_EXPORT void gm_module_set_icon_size(int width, int height)
{
    main_button_width = width;
    main_button_height = height;
}

G_MODULE_EXPORT GtkWidget *gm_module_get_widget()
{
    return GTK_WIDGET(main_button);
}

