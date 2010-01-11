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

static GtkWidget *event_box = NULL;
static int event_box_width = 50;
static int event_box_height = 50;
static int current_status = -2;
static nm_elements* stati = NULL;
static nm_elements* actions = NULL;
static const char* conffile = "./applets/netman/xml-config-files/netman.xml";
static GtkWidget *image_success = NULL;
static GtkWidget *image_fail = NULL;
static char **args;

static gint check_network_status()
{
  int status = -1;
  int ret;
  __pid_t childpid;
  FILE *fp;

  
  fp = fopen((char *) stati->exec,"r");
  if( fp )
  {
		fclose(fp);

    childpid = fork();
    if ( childpid == 0 )
    {
      execvp((char *) stati->exec, args);
      _exit(0);
		}

		while( status == -1 )
		{
			waitpid(childpid, &status, WNOHANG); 
			sleep(1);
		} 

		printf("check_network: returned status %d: current_status %d\n", status, current_status);
		if ( current_status != status )
		{
			current_status = status;
			if( status == 0 )
			{
				gtk_container_remove(GTK_CONTAINER(event_box), image_fail);
				gtk_container_add(GTK_CONTAINER(event_box), image_success);
				gtk_widget_show(image_success);
				return TRUE;
			}
			else
			{
				gtk_container_remove(GTK_CONTAINER(event_box), image_success);
				gtk_container_add(GTK_CONTAINER(event_box), image_fail);
				gtk_widget_show(image_fail);
				return FALSE;
			}
		}
	}
	else
	{
		g_warning("Could not open %s\n", stati->exec);
	}
}

G_MODULE_EXPORT int gm_module_init()
{
  int i;

	nm_load_conf(conffile);

	stati = nm_get_stati();
	actions = nm_get_actions();

	event_box = gtk_event_box_new();

	image_fail = gm_load_image((char*) stati->name, (char*) stati->logofail, nm_get_cache_location(), "netman-fail", event_box_width, event_box_height);	
	image_success = gm_load_image((char*) stati->name, (char*) stati->logosuccess, nm_get_cache_location(), "netman-success", event_box_width, event_box_height);	
	gtk_container_add(GTK_CONTAINER(event_box), image_fail);
	gtk_widget_show(image_fail);
	/*g_signal_connect (G_OBJECT (event_box),
                      "event_box_press_event",
                      G_CALLBACK (event_box_press_callback),
                      image);
	*/

	/**
    Create argument list. First element should be the filename
    of the executable and last element needs to be NULL.
    see man exec for more details
  */
  args = (char **) malloc((stati->numArguments + 2)* sizeof(char *));
  args[0] = (char *) stati->exec;
  for (i = 0; i < stati->numArguments; i++ )
  {
    args[i+1] = stati->args[i];
  }
  args[i+1] = NULL;

	return 0;
}

G_MODULE_EXPORT void gm_module_set_conffile(const char* filename)
{
	conffile = filename;
}

G_MODULE_EXPORT GThreadFunc gm_module_start()
{
	while(1)
	{
		check_network_status();
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
	event_box_width = width;
	event_box_height = height;
}

G_MODULE_EXPORT GtkWidget *gm_module_get_widget()
{
	return event_box;
}
