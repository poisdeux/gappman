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

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <glib/gprintf.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>
#include <getopt.h>
#include <string.h>
#include <gm_parseconf.h>
#include <gm_layout.h>
#include <sys/types.h>
#include <signal.h>
#include <gm_connect.h>
#include "connect.h"

static int WINDOWED = 0;
static GtkWidget *mainwin;
static GtkWidget *killdialogwin;
static char* color[4] = {"green", "orange", "red", "yellow"};
static char* statusarray[5] =  {"running", "sleeping", "stopped", "waiting", "zombie"};  
static int fontsize;

static void usage()
{
  printf("usage: processmanager [--help] [--width <WIDTHINPIXELS>] [--height <HEIGHTINPIXELS>] [--conffile <FILENAME>] [--gtkrc <GTKRCFILENAME>] [--windowed]\n");
  printf("");
  printf("--help:\t\tshows this help text\n");
  printf("--width <WIDTHINPIXELS>:\t\twidth of the main window (default: screen width / 9)\n");
  printf("--height <HEIGHTINPIXELS:\t\theight of the main window (default: screen height / 9)\n");
  printf("--conffile <FILENAME>:\t\t configuration file specifying the program and actions (default: /etc/gappman/processmanager.xml)\n");
  printf("--gtkrc <GTKRCFILENAME>:\t\t gtk configuration file which can be used for themeing\n");
  printf("--windowed:\t\t creates a border around the window\n");

}

static void destroy_widget( GtkWidget *widget, gpointer data )
{
	gtk_widget_destroy(GTK_WIDGET(data));
}


static int get_status(int PID)
{
  char* proc_string = malloc((strlen("/proc/stat") + 6) * sizeof(char));
  gchar* contents = NULL;
  gchar** contentssplit = NULL;
  gsize length;
  GError *gerror = NULL;
	gchar* status = NULL;
	int ret_status = -1;

  g_sprintf(proc_string, "/proc/%d/stat", PID);
  if ( ! g_file_get_contents(proc_string, &contents, &length, &gerror))
  {
    g_warning("gappman (get_started_apps): %s\n", gerror->message );
		ret_status = -2;
  }
  else
  {
    contentssplit = g_strsplit(contents, " ", 4);

   	status = g_strdup(contentssplit[2]);

		//RUNNING
		if ( g_strcmp0("R", status) == 0 )
		{
			ret_status = -2; 
		}
		//ZOMBIE
		else if ( g_strcmp0("Z", status) == 0 )
		{
			ret_status = 4;
		}
		//SLEEPING
		else if ( g_strcmp0("S", status) == 0 )
		{
			ret_status = 1;
		}
		//WAITING UNINTERRUPTIBLE DISK SLEEP
		else if ( g_strcmp0("D", status) == 0 )
		{
			ret_status = 3;
		}
		//TRACED OR STOPPED
		else if ( g_strcmp0("T", status) == 0 )
		{
			ret_status = 2;
		}
		//PAGING
		else if ( g_strcmp0("W", status) == 0 )
		{
			ret_status = 3;
		}
		g_free(contents);
    g_strfreev(contentssplit);
  }
	return ret_status;
}

static int kill_program( GtkWidget *widget, menu_elements *elt )
{
	int status;
	int count = 0;

	status = get_status(elt->pid);
	switch(status)
	{
		case 0|1:
			kill(elt->pid, 15);
			break;;
		case 2|3:
			kill(elt->pid, 9);
			break;;
		case 4:
			g_debug("Oh my god! It's a zombie. Hit it with a shovel!\n");
			return FALSE;;
		case -2:
			g_warning("Oops, could not find proces %d\n", elt->pid);
			return TRUE;;
		case -1:
			g_warning("Huh? What should I do with status %d for proces %d\n", status, elt->pid);
			return FALSE;;
	}

	//disable kill button
	gtk_widget_set_sensitive(widget, FALSE);

	while((get_status(elt->pid) > 0) && (count < 10))
	{
		sleep(1);
		count++;
	}	
	if( count == 10 )		
	{
		if(status != get_status(elt->pid))
		{
			//Process changed status.
			//Let's try again
			kill_program(widget, elt);
		}
	}

	//If kill was succesful we should remove
	//program from main window
	gtk_widget_set_sensitive(elt->widget, FALSE);

	destroy_widget(widget, killdialogwin);

	return TRUE;
}


/**
* \brief creates a popup dialog window that allows the user to stop a program
* \param *elt pointer to menu_element structure that contains the program to be stopped
*/
static void showprocessdialog( menu_elements *elt )
{
	GtkWidget *button, *buttonbox, *label;
	gchar* markup;

	killdialogwin = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  	gtk_window_set_transient_for (GTK_WINDOW(killdialogwin), GTK_WINDOW(mainwin));
	gtk_window_set_position(GTK_WINDOW (killdialogwin), GTK_WIN_POS_CENTER_ON_PARENT);
 
  //Make window transparent
  //gtk_window_set_opacity (GTK_WINDOW (killdialogwin), 0.8);
  
  //Remove border
  gtk_window_set_decorated (GTK_WINDOW (killdialogwin), FALSE);

  buttonbox = gtk_hbutton_box_new();	
	
  label = gtk_label_new("");	
  markup = g_markup_printf_escaped ("<span size=\"%d\">%s</span>", fontsize, g_strdup_printf("Stop %s", elt->name));
  gtk_label_set_markup (GTK_LABEL (label), markup);
	g_free (markup);
	button = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(button), label);
  gtk_widget_show(label);
 	g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (kill_program), elt);
	gtk_container_add(GTK_CONTAINER(buttonbox), button);
	gtk_widget_show(button);

	button = gm_create_cancel_button(destroy_widget, killdialogwin);
  gtk_widget_show(label);
  gtk_container_add(GTK_CONTAINER(buttonbox), button);
  gtk_widget_show(button);

  gtk_container_add(GTK_CONTAINER(killdialogwin), buttonbox);
  gtk_widget_show(buttonbox);
  gtk_widget_show(killdialogwin);
  gtk_widget_grab_focus(button);
}

/**
* \brief function that starts any program as defined by the structure *elt.
* \param *widget pointer to the button widget which has the process_startprogram_event connected through a signal
* \param *event the GdkEvent that occured. Space key and left mousebutton are valid actions.
* \param *elt menu_element structure containing the filename and arguments of the program that should be started
*/
static gboolean process_startprogram_event ( GtkWidget *widget, GdkEvent *event, menu_elements *elt )
{

  //Only start program  if spacebar or mousebutton is pressed
  if( ((GdkEventKey*)event)->keyval == 32 || ((GdkEventButton*)event)->button == 1)
  {
		gtk_widget_set_sensitive(GTK_WIDGET(widget), FALSE);	
    showprocessdialog( elt );
		gtk_widget_set_sensitive(GTK_WIDGET(widget), TRUE);	
  }

  return TRUE;
}


static GtkWidget* createrow(menu_elements *elt, int width, int height)
{
  GtkWidget *hbox, *imagebox, *statuslabel;
	gchar *markup;
	GtkWidget *alignment;
	int status;
	
	hbox = gtk_hbox_new (FALSE, 10);

  elt->widget = gm_create_button(elt, fontsize, width, height, process_startprogram_event);

	statuslabel = gtk_label_new("");
	status = get_status(elt->pid);

	markup = g_markup_printf_escaped ("<span size=\"%d\" foreground=\"%s\">%s</span>", fontsize, color[status], statusarray[status]);
	gtk_label_set_markup (GTK_LABEL (statuslabel), markup);
	g_free (markup);

	//right justify the labeltext
	alignment = gtk_alignment_new(1.0, 0.5, 0, 0);
	gtk_container_add(GTK_CONTAINER(alignment), statuslabel);
	gtk_widget_show(statuslabel);
	
	gtk_container_add(GTK_CONTAINER(hbox), elt->widget);
	gtk_widget_show(elt->widget);	
	gtk_container_add(GTK_CONTAINER(hbox), alignment);
	gtk_widget_show(alignment);

	return hbox;
}

/**
* \brief callback function to quit the program
* \param *widget pointer to widget to destroy
* \param data mandatory argument for callback function, may be NULL.
*/
static void destroy( GtkWidget *widget,
                     gpointer   data )
{
    gtk_main_quit ();
}

/**
* \brief main function setting up the UI
*/
int main (int argc, char **argv)
{
  GdkScreen *screen;
  GdkWindow *rootwin;
  GtkWidget *button;
  GtkWidget *labelimagebox;
  GtkWidget *vbox;
  GtkWidget *hbox;
  GtkWidget *align;
  GtkWidget *separator;
  GtkWidget *table;
	menu_elements *program_elts = NULL;
	menu_elements *action_elts = NULL;
  const char* conffile = "/etc/gappman/processmanager.xml";
  int dialog_width;
  int dialog_height;
  int program_width;
  int no_progsacts_found = 1;  //< used to determine if any progs were started by gappman. If this program (processmanager) was started using gappman.
  int row_height;
  int total_amount_of_elements = 1;
  int status;
  int c;
  int mypid;
  time_t timestruct;
	struct proceslist* started_procs = NULL;
	struct proceslist* started_procs_tmp = NULL;

	mypid = getpid();
  gtk_init (&argc, &argv);
  screen = gdk_screen_get_default ();
  dialog_width =  gdk_screen_get_width (screen)/3;
  dialog_height =  gdk_screen_get_height (screen)/3;

  mainwin = gtk_window_new (GTK_WINDOW_TOPLEVEL);
 
  while (1) {
      int option_index = 0;
      static struct option long_options[] = {
          {"width", 1, 0, 'w'},
          {"height", 1, 0, 'h'},
          {"conffile", 1, 0, 'c'},
          {"help", 0, 0, 'i'},
          {"gtkrc", 1, 0, 'r'},
          {"windowed", 0, 0, 'j'},
          {0, 0, 0, 0}
      };
      c = getopt_long(argc, argv, "w:h:c:d:r:ij",
              long_options, &option_index);
      if (c == -1)
          break;

      switch (c) {
      case 'w':
          dialog_width=atoi(optarg);
          break;
      case 'h':
          dialog_height=atoi(optarg);
          break;
      case 'c':
          conffile=optarg;
          break;
      case 'r':
          gtk_rc_parse (optarg);
          break;
      case 'j':
          WINDOWED=1;
          break;
      default:
          usage();
          return 0;
      }
  }

 	gtk_window_set_position(GTK_WINDOW (mainwin), GTK_WIN_POS_CENTER);
	//Remove border
  if ( !WINDOWED )
  {
    gtk_window_set_decorated (GTK_WINDOW (mainwin), FALSE);
  }
  else
  {
    gtk_window_set_decorated (GTK_WINDOW (mainwin), TRUE);
    g_signal_connect (G_OBJECT (mainwin), "delete_event",
          G_CALLBACK (destroy), NULL);
    g_signal_connect (G_OBJECT (mainwin), "destroy",
                      G_CALLBACK (destroy), NULL);
  }

	status = gm_get_fontsize_from_gappman(2103, "localhost", &fontsize);;
	if (status == 0)
	{
		gm_set_fontsize(fontsize);
	}
	else
	{
		//fallback on default
		fontsize=gm_get_fontsize();
	}	
	status = getStartedProcsFromGappman(2103, "localhost", &started_procs);

	switch(status)
	{
		case 0:
			break;;
		case 1:
			gm_show_error_dialog("Could not resolve hostname: localhost", mainwin, destroy);
			break;;
		case 2:
			gm_show_error_dialog("Could not connect to gappman.\nCheck that gappman is running.", mainwin, destroy);
			break;;
		case 3:
			gm_show_error_dialog("Could not sent message to localhost.\nCheck that gappman is running", mainwin, destroy);
			break;;
		case 4:
			gm_show_error_dialog("Could not disconnect from gappman.", mainwin, destroy);
			break;;
		default:
			break;;
	}

	vbox = gtk_vbox_new(FALSE, 10);

	if( started_procs != NULL )
	{
		if ( gm_load_conf(conffile) != 0 )
		{	
			gm_show_error_dialog("Could not load gappman configuration file\n", mainwin, destroy);
		}
		else	
		{
			program_elts = gm_get_programs();
			if(program_elts != NULL)
			{
				//max width is 50% of screen width	
				program_width=dialog_width/2;
				total_amount_of_elements += *program_elts->amount_of_elements;
			}

			action_elts  = gm_get_actions();
			if(action_elts != NULL)
			{	
				total_amount_of_elements += *action_elts->amount_of_elements;
			}
			
			row_height=dialog_height/total_amount_of_elements;
			
			while  ( program_elts != NULL )
			{
				started_procs_tmp	 = started_procs;
				while ( started_procs_tmp != NULL)
				{
					if(( g_strcmp0(program_elts->name, started_procs_tmp->name) == 0 ) && ( started_procs_tmp->pid != mypid ))
					{
						no_progsacts_found = 0;
						program_elts->pid	 = started_procs_tmp->pid;
						hbox	 = createrow(program_elts, program_width, row_height);
						gtk_container_add(GTK_CONTAINER(vbox),	 hbox);	
						gtk_widget_show	 (hbox);
						separator	 = gtk_hseparator_new();
						gtk_container_add(GTK_CONTAINER(vbox),	 separator);	
						gtk_widget_show	 (separator);
	
						//get	 out of while-loop
						started_procs_tmp	 = NULL;
					}
					else
					{
							started_procs_tmp	 = started_procs_tmp->prev;
					}
				}	
				program_elts	 = program_elts->next;
			}	
		
			while ( action_elts != NULL )
			{
				started_procs_tmp = started_procs;
				while ( started_procs_tmp != NULL)
				{
					if(( g_strcmp0(action_elts->name, started_procs_tmp->name) == 0 ) && ( started_procs_tmp->pid != mypid))
					{
						no_progsacts_found = 0;
						action_elts->pid = started_procs_tmp->pid;
		
						hbox = createrow(action_elts, program_width, row_height);
						gtk_container_add(GTK_CONTAINER(vbox), hbox);	
						gtk_widget_show (hbox);
						separator = gtk_hseparator_new();
						gtk_container_add(GTK_CONTAINER(vbox), separator);	
						gtk_widget_show (separator);
		
						//get out of while-loop
						started_procs_tmp = NULL;
					}
					else
					{
						started_procs_tmp = started_procs_tmp->prev;
					}
				}	
			  action_elts = action_elts->next;
			}
			freeproceslist(started_procs);
			if( no_progsacts_found == 0 )
			{	
				hbox = gtk_hbox_new (FALSE, 10);
				// cancel button
				button = gm_create_cancel_button(gtk_main_quit, NULL);
				  gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);
		  		gtk_widget_show(button);
		
				gtk_container_add(GTK_CONTAINER(vbox), hbox);
				gtk_widget_show (hbox);	
				gtk_container_add (GTK_CONTAINER (mainwin), vbox);
				  gtk_widget_show (vbox);
		 		 gtk_widget_show (mainwin);
			}
			else
			{
				gm_show_error_dialog("No programs started by gappman.", mainwin, destroy);
			}
	    	}
	}
	else
	{
		gm_show_error_dialog("No programs started by gappman.", mainwin, destroy);
	}
	
  	gtk_main ();

	gm_free_menu_elements( program_elts );
	gm_free_menu_elements( action_elts );
  return 0;
}

