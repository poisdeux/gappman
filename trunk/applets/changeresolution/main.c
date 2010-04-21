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
#include <sys/types.h>
#include <signal.h>
#include <gm_layout.h>
#include <gm_connect.h>
#include <gm_changeresolution.h>
#include <gm_generic.h>

static int WINDOWED = 0;
static GtkWidget *mainwin;
static int fontsize;
static menu_elements *programs;
static int dialog_width;
static int dialog_height;

static void usage()
{
  printf("usage: changeresolution [--help] [--screenwidth <WIDTHINPIXELS>] [--screenheight <HEIGHTINPIXELS>] [--gmconffile <FILENAME>] [--gtkrc <GTKRCFILENAME>] [--windowed]\n");
  printf("");
  printf("--help:\t\tshows this help text\n");
  printf("--screenwidth <WIDTHINPIXELS>:\t\twidth of the main (gappman) window (default: screen width / 3)\n");
  printf("--screenheight <HEIGHTINPIXELS:\t\theight of the main (gappman) window (default: screen height / 3)\n");
  printf("--gmconffile <FILENAME>:\t\t configuration file specifying the program and actions (default: /etc/gappman/conf.xml)\n");
  printf("--gtkrc <GTKRCFILENAME>:\t\t gtk configuration file which can be used for themeing\n");
  printf("--windowed:\t\t creates a border around the window\n");

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


static void destroy_widget( GtkWidget *widget, gpointer data )
{
	gtk_widget_destroy(GTK_WIDGET(data));
}

static gboolean revert_to_old_res(GtkWidget *widget, XRRScreenSize* size)
{
	gm_changeresolution(size->width, size->height);
	return FALSE;
}

static gboolean set_default_res_for_program( GtkWidget *widget, GdkEvent *event, menu_elements *elt )
{
  XRRScreenSize current_size;
  gchar *msg;
  //Check if spacebar or mousebutton is pressed
  if( ((GdkEventKey*)event)->keyval == 32 || ((GdkEventButton*)event)->button == 1)
  {
	gm_get_current_size(&current_size);
  	msg = g_strdup_printf("::updateres::%s::%d::%d::", elt->name, current_size.width, current_size.height);
	g_debug("Setting default resolution for %s\n", elt->name);
	if( gm_send_and_receive_message(2103, "localhost", msg, NULL) != GM_SUCCES )
	{
		gm_show_error_dialog("Could not connect to gappman.", NULL, NULL);
	}
	return TRUE;
  }
}

static void make_default_for_program( XRRScreenSize *size )
{
	GtkWidget *button;
	GtkWidget *chooseprogramwin;
	GtkWidget *vbox;
	int button_height;
	menu_elements *programs_tmp;

	chooseprogramwin = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  	gtk_window_set_transient_for (GTK_WINDOW(chooseprogramwin), GTK_WINDOW(mainwin));
	gtk_window_set_position(GTK_WINDOW (chooseprogramwin), GTK_WIN_POS_CENTER_ON_PARENT);
 
  	//Make window transparent
  	//gtk_window_set_opacity (GTK_WINDOW (chooseprogramwin), 0.8);
  
  	//Remove border
	gtk_window_set_decorated (GTK_WINDOW (chooseprogramwin), FALSE);

	programs_tmp = programs;
	if (programs_tmp != NULL)
	{
		vbox = gtk_vbox_new(FALSE, 10);	
		button_height = dialog_height/(*programs_tmp->amount_of_elements);
		while(programs_tmp != NULL)
		{
			button = gm_create_button(programs_tmp, fontsize, dialog_width, button_height, set_default_res_for_program);
			gtk_container_add(GTK_CONTAINER(vbox), button);
			programs_tmp = programs_tmp->next;
		}
		button = gm_create_cancel_button(destroy_widget, chooseprogramwin); 
		gtk_container_add(GTK_CONTAINER(vbox), button);
		gtk_container_add(GTK_CONTAINER(chooseprogramwin), vbox);
		gtk_widget_show_all(chooseprogramwin);	
	}
	else
	{
		gm_show_error_dialog("No programs found.\nPlease check configuration file.", NULL, NULL);
	}
}

/**
* \brief creates a popup dialog window that allows the user confirm the new resolution 
* \param *size pointer to a XRRScreenSize struct that holds the new resolution
*/
static void changeresolution( XRRScreenSize *size )
{
	GtkWidget *button;
	GtkWidget *vbox;
	GtkWidget *label;
	GtkWidget *confirmwin;
	gchar* markup;
	static XRRScreenSize oldsize;
	int nr;
	
	gm_get_current_size(&oldsize);

	gm_changeresolution(size[0].width, size[0].height);
	
	confirmwin = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  	gtk_window_set_transient_for (GTK_WINDOW(confirmwin), GTK_WINDOW(mainwin));
	gtk_window_set_position(GTK_WINDOW (confirmwin), GTK_WIN_POS_CENTER_ON_PARENT);
 
  //Make window transparent
  //gtk_window_set_opacity (GTK_WINDOW (confirmwin), 0.8);
  
  //Remove border
  gtk_window_set_decorated (GTK_WINDOW (confirmwin), FALSE);

	vbox = gtk_vbox_new(TRUE, 10);	

	label = gtk_label_new("");	
	markup = g_markup_printf_escaped ("<span size=\"%d\">Set current resolution as default for program.</span>", fontsize);
	gtk_label_set_markup (GTK_LABEL (label), markup);
	g_free (markup);
	button = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(button), label);
 	g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (make_default_for_program), size);
	gtk_container_add(GTK_CONTAINER(vbox), button);

	label = gtk_label_new("");	
	markup = g_markup_printf_escaped ("<span size=\"%d\">Keep resolution just this once.</span>", fontsize);
	gtk_label_set_markup (GTK_LABEL (label), markup);
	g_free (markup);
	button = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(button), label);
 	g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (destroy_widget), confirmwin);
	gtk_container_add(GTK_CONTAINER(vbox), button);

	button = gm_create_cancel_button(revert_to_old_res, &oldsize); 
 	g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (destroy_widget), confirmwin);
  	gtk_container_add(GTK_CONTAINER(vbox), button);

	gtk_container_add(GTK_CONTAINER(confirmwin), vbox);
	gtk_widget_grab_focus(button);

	gtk_widget_show_all(confirmwin);	

	//We add a timer to return to the original resolution
	//after 10 seconds
	//g_timeout_add_seconds (10, )
}

/**
* \brief function that starts any program as defined by the structure *elt.
* \param *widget pointer to the button widget which has the process_startprogram_event connected through a signal
* \param *event the GdkEvent that occured. Space key and left mousebutton are valid actions.
* \param *elt menu_element structure containing the filename and arguments of the program that should be started
*/
static gboolean process_startprogram_event ( GtkWidget *widget, GdkEvent *event, void *size )
{
  //Only start program  if spacebar or mousebutton is pressed
  if( ((GdkEventKey*)event)->keyval == 32 || ((GdkEventButton*)event)->button == 1)
  {
		gtk_widget_set_sensitive(GTK_WIDGET(widget), FALSE);	
		changeresolution(size);
		gtk_widget_set_sensitive(GTK_WIDGET(widget), TRUE);	
  }

  return TRUE;
}


static GtkWidget* createrow(XRRScreenSize* size, int width, int height)
{
  GtkWidget *button, *hbox, *label;
	gchar *markup;
	GtkWidget *alignment;
	int status;
	
	hbox = gtk_hbox_new (FALSE, 10);

	// Need to expand menu_elt structure to contain PID status.
  button = gm_create_empty_button(width, height, process_startprogram_event, size);

	label = gtk_label_new("");

	markup = g_markup_printf_escaped ("<span size=\"%d\">%dx%d</span>", fontsize, size->width, size->height);
	gtk_label_set_markup (GTK_LABEL (label), markup);
	g_free (markup);
	gtk_container_add(GTK_CONTAINER(button), label);
  gtk_widget_show(label);
	gtk_container_add(GTK_CONTAINER(hbox), button);
	gtk_widget_show(button);	
	//gtk_container_add(GTK_CONTAINER(hbox), alignment);
	//gtk_widget_show(alignment);

	return hbox;
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

	int row_height;
  int c;
	int nsize;
	int ret_value;
	int i;
	XRRScreenSize *sizes;
	const char* conffile = "/etc/gappman/conf.xml";

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
      c = getopt_long(argc, argv, "w:h:c:r:ij",
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
	
	//get generic fontsize from gappman
	if(gm_get_fontsize_from_gappman(2103, "localhost", &fontsize) == GM_SUCCES)
  {
    gm_set_fontsize(fontsize);
  }
  else
	{
		fontsize = gm_get_fontsize();
	}

	gm_load_conf(conffile);
	programs = gm_get_programs();

	ret_value = gm_getpossibleresolutions(&sizes, &nsize);
	if(ret_value != GM_SUCCES)
	{
		g_warning("Error could not get possible resolutions (error_type: %d)\n", ret_value); 
		gm_show_error_dialog("Changing screen resolution is not supported.", mainwin, destroy);
	}
	else
	{

	 	//Make window transparent
 	 	//gtk_window_set_opacity (GTK_WINDOW (mainwin), 0.8);
 	
		vbox = gtk_vbox_new(FALSE, 10);

		for(i = 0; i < nsize; i++)
		{
			hbox = createrow(&sizes[i], dialog_width, row_height);
			gtk_container_add(GTK_CONTAINER(vbox), hbox);	
			gtk_widget_show (hbox);
			separator = gtk_hseparator_new();
			gtk_container_add(GTK_CONTAINER(vbox), separator);	
			gtk_widget_show (separator);
		}
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
	

  gtk_main ();

  return 0;
}

