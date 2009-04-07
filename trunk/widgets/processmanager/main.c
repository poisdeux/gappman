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

static GtkWidget* createrow(menu_elements *elt, gchar *status, int status_fontsize, int width, int height)
{
  GtkWidget *hbox, *imagebox, *statuslabel;
	gchar *markup;

	hbox = gtk_hbox_new (FALSE, 10);

	//status_fontsize is 1024th of a point. We reserve 20% of the total width
	//for the status column.
	imagebox = image_label_box_vert (elt, (gchar *) elt->name, width, height);
	statuslabel = gtk_label_new("");
	markup = g_markup_printf_escaped ("<span size=\"%d\">%s</span>", status_fontsize, status);
	gtk_label_set_markup (GTK_LABEL (statuslabel), markup);
	g_free (markup);

	
	gtk_container_add(GTK_CONTAINER(hbox), imagebox);
	gtk_widget_show(imagebox);	
	gtk_container_add(GTK_CONTAINER(hbox), statuslabel);
	gtk_widget_show(statuslabel);
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
  GtkWidget *mainwin;
  GtkWidget *table;
  menu_elements *programs, *elt_tmp;
  const char* conffile = "/etc/gappman/processmanager.xml";
  int dialog_width;
  int dialog_height;
	int program_width;
	int row_height;
  int c;
	int status_fontsize;
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
	status_fontsize=1024*(dialog_width/2)/8;
	program_width=dialog_width-(dialog_width/2);
	row_height=dialog_height/getNumberOfElements();
	
	vbox = gtk_vbox_new(FALSE, 10);

	elt_tmp = programs;

	while ( programs != NULL )
	{
		hbox = createrow(programs, "running", status_fontsize, program_width, dialog_height);

		gtk_container_add(GTK_CONTAINER(vbox), hbox);	
		gtk_widget_show (hbox);
	
		programs = programs->next;		
	}	

 	gtk_container_add (GTK_CONTAINER (mainwin), vbox);
  gtk_widget_show (vbox);
  gtk_widget_show (mainwin);

  gtk_main ();

	freeMenuElements(elt_tmp);
  return 0;
}

