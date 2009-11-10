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
#include <layout.h>
#include <gm_connect.h>
#include <changeresolution.h>

static int WINDOWED = 0;
static GtkWidget *mainwin;
static int fontsize;

static void usage()
{
  printf("usage: changeresolution [--help] [--screenwidth <WIDTHINPIXELS>] [--screenheight <HEIGHTINPIXELS>] [--conffile <FILENAME>] [--gtkrc <GTKRCFILENAME>] [--windowed]\n");
  printf("");
  printf("--help:\t\tshows this help text\n");
  printf("--screenwidth <WIDTHINPIXELS>:\t\twidth of the main (gappman) window (default: screen width / 3)\n");
  printf("--screenheight <HEIGHTINPIXELS:\t\theight of the main (gappman) window (default: screen height / 3)\n");
  printf("--conffile <FILENAME>:\t\t configuration file specifying the program and actions (default: /etc/gappman/processmanager.xml)\n");
  printf("--gtkrc <GTKRCFILENAME>:\t\t gtk configuration file which can be used for themeing\n");
  printf("--windowed:\t\t creates a border around the window\n");

}

static void destroy_widget( GtkWidget *widget, gpointer data )
{
	gtk_widget_destroy(GTK_WIDGET(data));
}

static gboolean revert_to_old_res(GtkWidget *widget, XRRScreenSize *size)
{
	printf("changeresolution: revert_to_old_res\n");
	printf("DEBUG: %p => %dx%d\n", size, size->width, size->height);
	fflush(stdout);
	gm_changeresolution(size->width, size->height);
	return FALSE;
}

/**
* \brief creates a popup dialog window that allows the user to stop a program
* \param *elt pointer to menu_element structure that contains the program to be stopped
*/
static void changeresolution( XRRScreenSize *size )
{
	GtkWidget *button, *buttonbox, *label, *confirmwin;
	gchar* markup;
	XRRScreenSize *oldsize;
	int nr;

	if (gm_getpossibleresolutions(&oldsize, &nr) != SUCCES)
	{
		//could not get current resolution so bailing out
		return;
	}
	printf("DEBUG: %p => %dx%d\n", size, size->width, size->height);
	gm_changeresolution(size->width, size->height);
	
	confirmwin = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  gtk_window_set_transient_for (GTK_WINDOW(confirmwin), GTK_WINDOW(mainwin));
	gtk_window_set_position(GTK_WINDOW (confirmwin), GTK_WIN_POS_CENTER_ON_PARENT);
 
  //Make window transparent
  //gtk_window_set_opacity (GTK_WINDOW (confirmwin), 0.8);
  
  //Remove border
  gtk_window_set_decorated (GTK_WINDOW (confirmwin), FALSE);

	buttonbox = gtk_hbutton_box_new();	
	
	label = gtk_label_new("");	
	markup = g_markup_printf_escaped ("<span size=\"%d\">Keep resolution</span>", fontsize);
	gtk_label_set_markup (GTK_LABEL (label), markup);
	g_free (markup);
	button = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(button), label);
  gtk_widget_show(label);
 	g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (destroy_widget), confirmwin);
	gtk_container_add(GTK_CONTAINER(buttonbox), button);
	gtk_widget_show(button);

	label = gtk_label_new("");	
	markup = g_markup_printf_escaped ("<span size=\"%d\">Cancel</span>", fontsize);
	gtk_label_set_markup (GTK_LABEL (label), markup);
	g_free (markup);
	button = gtk_button_new();
 	g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (revert_to_old_res), &oldsize[0]);
 	g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (destroy_widget), confirmwin);
	gtk_container_add(GTK_CONTAINER(button), label);
  gtk_widget_show(label);
  gtk_container_add(GTK_CONTAINER(buttonbox), button);
  gtk_widget_show(button);

	gtk_container_add(GTK_CONTAINER(confirmwin), buttonbox);
	gtk_widget_show(buttonbox);
	gtk_widget_show(confirmwin);
	gtk_widget_grab_focus(button);
	
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
  button = create_empty_button(width, height, process_startprogram_event, size);

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
	GtkWidget *dialog;
  GtkWidget *labelimagebox;
  GtkWidget *vbox;
  GtkWidget *hbox;
  GtkWidget *align;
  GtkWidget *separator;
  int dialog_width;
  int dialog_height;
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
	if(getFontsizeFromGappman(2103, "localhost", &fontsize) == 0)
  {
    gm_set_fontsize(fontsize);
  }
	else
	{
		fontsize = gm_get_fontsize();
	}
	
	ret_value = gm_getpossibleresolutions(&sizes, &nsize);
	if(ret_value != SUCCES)
	{
		g_warning("Error could not get possible resolutions (error_type: %d)\n", ret_value); 
 		dialog = gtk_message_dialog_new_with_markup (GTK_WINDOW(mainwin), 
			GTK_DIALOG_DESTROY_WITH_PARENT, 
			GTK_MESSAGE_ERROR, 
			GTK_BUTTONS_CLOSE, 
			"<span size=\"%d\">Changing screen resolution is not supported.</span>", fontsize);
 		g_signal_connect_swapped (dialog, "response",
                           G_CALLBACK (destroy),
                           dialog);
	
  	gtk_widget_show (dialog);
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
 		button = gm_create_cancel_button(gtk_main_quit); 
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

