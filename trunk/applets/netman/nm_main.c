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

static GtkWidget *button = NULL;
static int button_width = 50;
static int button_height = 50;
static nm_elements* stati = NULL;
static nm_elements* actions = NULL;
static const char* conffile = "./applets/netman/xml-config-files/netman.xml";
GtkWidget *image_success = NULL;
GtkWidget *image_fail = NULL;

static gint check_network_status()
{
	char **args;
  int i;
  int status;
  int ret;
  __pid_t childpid;
  FILE *fp;

	sleep(200);
	return TRUE;
  /**
    Create argument list. First element should be the filename
    of the executable and last element needs to be NULL.
    see man exec for more details
  */
  args = (char **) malloc((stati->numArguments + 2)* sizeof(char *));
  args[0] = (char *) stati->exec;
  for ( i = 0; i < stati->numArguments; i++ )
  {
    args[i+1] = stati->args[i];
  }
  args[i+1] = NULL;

	printf("check_network: 1\n");
  fp = fopen((char *) stati->exec,"r");
  if( fp )
  {
		fclose(fp);
    childpid = fork();
    if ( childpid == 0 )
    {
			printf("DEBUG: Executing: %s with args: ", stati->exec);
        for ( i = 0; i < stati->numArguments; i++ )
        {
          printf("%s ", args[i+1]);
        }
      execvp((char *) stati->exec, args);
      _exit(0);
		}
		wait(&status);
		printf("check_network: returned status %d\n", status);
		if( status == 0 )
		{
			gtk_button_set_image(GTK_BUTTON(button), image_success);
		}
		else
		{
			gtk_button_set_image(GTK_BUTTON(button), image_fail);
		}
	}
	else
	{
		g_warning("Could not open %s\n", stati->exec);
	}
	free(args);
	printf("TEST: end of check_network\n");
	return TRUE;
}

G_MODULE_EXPORT int gm_module_init()
{

	nm_load_conf(conffile);
	stati = nm_get_stati();
	actions = nm_get_actions();

	button = gtk_button_new();
	image_fail = gm_load_image((char*) stati->name, (char*) stati->logofail, nm_get_cache_location(), "netman", button_width, button_height);	
	gtk_button_set_image(GTK_BUTTON(button), image_fail);
  gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
	
	return 0;
}

G_MODULE_EXPORT void gm_module_set_conffile(const char* filename)
{
	conffile = filename;
}

G_MODULE_EXPORT GThreadFunc gm_module_start()
{
	//check_network_status();
	g_timeout_add(10000, (GSourceFunc) check_network_status, NULL);
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

