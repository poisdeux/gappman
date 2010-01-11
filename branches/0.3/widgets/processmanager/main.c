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
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>
#include <getopt.h>
#include <string.h>
#include <../../libs/parseconf/parseconf.h>
#include <../../libs/layout/layout.h>

static int DEBUG = 0;
static int WINDOWED = 0;
static GtkWidget *mainwin;
static char* color[3] = {"green", "orange", "red"};
static char* status[3] =  {"running", "sleeping", "stopped"};  
static int fontsize;

static void usage()
{
  printf("usage: processmanager [--help] [--debug <LEVEL>] [--width <WIDTHINPIXELS>] [--height <HEIGHTINPIXELS>] [--conffile <FILENAME>] [--gtkrc <GTKRCFILENAME>] [--windowed]\n");
  printf("");
  printf("--help:\t\tshows this help text\n");
  printf("--debug <LEVEL>:\t\tsets verbosity leven\n");
  printf("--width <WIDTHINPIXELS>:\t\twidth of the main window (default: screen width / 9)\n");
  printf("--height <HEIGHTINPIXELS:\t\theight of the main window (default: screen height / 9)\n");
  printf("--conffile <FILENAME>:\t\t configuration file specifying the program and actions (default: /etc/gappman/shutdown.xml)\n");
  printf("--gtkrc <GTKRCFILENAME>:\t\t gtk configuration file which can be used for themeing\n");
  printf("--windowed:\t\t creates a border around the window\n");

}

static void destroy_widget( GtkWidget *widget, gpointer data )
{
	gtk_widget_destroy(GTK_WIDGET(data));
}

/**
* \brief creates a popup dialog window that allows the user to stop a program
* \param *elt pointer to menu_element structure that contains the program to be stopped
*/
static void showprocessdialog( menu_elements *elt )
{
	GtkWidget *button, *buttonbox, *dialogwin, *label;
	gchar* markup;

	dialogwin = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  gtk_window_set_transient_for (GTK_WINDOW(dialogwin), GTK_WINDOW(mainwin));
	gtk_window_set_position(GTK_WINDOW (dialogwin), GTK_WIN_POS_CENTER_ON_PARENT);
 
  //Make window transparent
  //gtk_window_set_opacity (GTK_WINDOW (dialogwin), 0.8);
  
  //Remove border
  gtk_window_set_decorated (GTK_WINDOW (dialogwin), FALSE);

	buttonbox = gtk_hbutton_box_new();	

	label = gtk_label_new("");	
	markup = g_markup_printf_escaped ("<span size=\"%d\">%s</span>", fontsize, g_strdup_printf("Stop %s", elt->name));
	gtk_label_set_markup (GTK_LABEL (label), markup);
	g_free (markup);
	button = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(button), label);
  gtk_widget_show(label);
	gtk_container_add(GTK_CONTAINER(buttonbox), button);
	gtk_widget_show(button);

	label = gtk_label_new("");	
	markup = g_markup_printf_escaped ("<span size=\"%d\">Cancel</span>", fontsize);
	gtk_label_set_markup (GTK_LABEL (label), markup);
	g_free (markup);
	button = gtk_button_new();
 	g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (destroy_widget), dialogwin);
	gtk_container_add(GTK_CONTAINER(button), label);
  gtk_widget_show(label);
  gtk_container_add(GTK_CONTAINER(buttonbox), button);
  gtk_widget_show(button);

	gtk_container_add(GTK_CONTAINER(dialogwin), buttonbox);
	gtk_widget_show(buttonbox);
	gtk_widget_show(dialogwin);
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

  if (DEBUG == 2)
  {
    printf("process_startprogram_event\n");
  }

  if(DEBUG > 1)
  {
    if ( event->type == GDK_KEY_RELEASE )
    {
      printf("%s key pressed and has value %d\n", ((GdkEventKey*)event)->string, ((GdkEventKey*)event)->keyval);
    }
  }

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
  GtkWidget *button, *hbox, *imagebox, *statuslabel;
	gchar *markup;
	GtkWidget *alignment;
	
	hbox = gtk_hbox_new (FALSE, 10);

	// Need to expand menu_elt structure to contain PID status.
  button = createbutton(elt, fontsize, width, height, process_startprogram_event);

	statuslabel = gtk_label_new("");
	markup = g_markup_printf_escaped ("<span size=\"%d\" foreground=\"%s\">%s</span>", fontsize, color[elt->status], status[elt->status]);
	gtk_label_set_markup (GTK_LABEL (statuslabel), markup);
	g_free (markup);

	//right justify the labeltext
	alignment = gtk_alignment_new(1.0, 0.5, 0, 0);
	gtk_container_add(GTK_CONTAINER(alignment), statuslabel);
	gtk_widget_show(statuslabel);
	
	gtk_container_add(GTK_CONTAINER(hbox), button);
	gtk_widget_show(button);	
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
  menu_elements *programs, *elt_tmp;
  const char* conffile = "/etc/gappman/processmanager.xml";
  int dialog_width;
  int dialog_height;
	int program_width;
	int row_height;
  int c;
  time_t timestruct;

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
          {"debug", 1, 0, 'd'},
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
      case 'd':
          DEBUG=atoi(optarg);
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

	loadConf(conffile);
	programs = getPrograms();

	printMenuElements(programs);

  gtk_window_set_position(GTK_WINDOW (mainwin), GTK_WIN_POS_CENTER);
 
  //Make window transparent
  //gtk_window_set_opacity (GTK_WINDOW (mainwin), 0.8);
  
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

	//The size is 1024th of a point. We reserve 30% for the statuscolumn
	//so we have dialog_width/3 * 1024 points available for a max of 8
	//characters
	fontsize=1024*(dialog_width/2)/8;
	program_width=dialog_width-(dialog_width/2);
	row_height=dialog_height/getNumberOfElements();
	
	vbox = gtk_vbox_new(FALSE, 10);

	
	elt_tmp = programs;

	while ( programs != NULL )
	{
		programs->status = 1;
		hbox = createrow(programs, program_width, row_height);

		gtk_container_add(GTK_CONTAINER(vbox), hbox);	
		gtk_widget_show (hbox);
	
		programs = programs->next;
		separator = gtk_hseparator_new();
		gtk_container_add(GTK_CONTAINER(vbox), separator);	
		gtk_widget_show (separator);
		
	}	

  hbox = gtk_hbox_new (FALSE, 10);
  // cancel button
  button = gtk_button_new_with_label("Cancel");
  g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (gtk_main_quit), NULL);
  gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);
  gtk_widget_show(button);

	gtk_container_add(GTK_CONTAINER(vbox), hbox);
	gtk_widget_show (hbox);	
 	gtk_container_add (GTK_CONTAINER (mainwin), vbox);
  gtk_widget_show (vbox);
  gtk_widget_show (mainwin);

  gtk_main ();

	freeMenuElements(elt_tmp);
  return 0;
}
