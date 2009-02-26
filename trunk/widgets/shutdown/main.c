/***
 * \file appmanager.c
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
  printf("usage: shutdown [--help] [--debug <LEVEL>] [--width <WIDTHINPIXELS>] [--height <HEIGHTINPIXELS>] [--conffile <FILENAME>] [--gtkrc <GTKRCFILENAME>] [--windowed]\n");
  printf("");
  printf("--help:\t\tshows this help text\n");
  printf("--debug <LEVEL>:\t\tsets verbosity leven\n");
  printf("--width <WIDTHINPIXELS>:\t\twidth of the main window (default: screen width / 9)\n");
  printf("--height <HEIGHTINPIXELS:\t\theight of the main window (default: screen height / 9)\n");
  printf("--conffile <FILENAME>:\t\t configuration file specifying the program and actions (default: /etc/gappman/shutdown.xml)\n");
  printf("--gtkrc <GTKRCFILENAME>:\t\t gtk configuration file which can be used for themeing\n");
  printf("--windowed:\t\t creates a border around the window\n");

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
    //startprogram( widget, elt );
  }

  return FALSE;
}

static GtkWidget* load_image(char* imagefile, int max_width, int max_height)
{
  GtkWidget *image;
  GdkPixbuf *pixbuf;
  int width,height;
  double ratio;

  image = gtk_image_new_from_file (imagefile);
  pixbuf = gtk_image_get_pixbuf(GTK_IMAGE(image));
  width = gdk_pixbuf_get_width(pixbuf);
  height = gdk_pixbuf_get_height(pixbuf);
  ratio = (double) width/max_width;
  width /= ratio;
  height /= ratio;

  // Check if button does not overlap the maximum allowed height
  // if so we assume orientation is portrait and determine
  // button size based on height
  if( height > max_height )
  {
    ratio = (double) height/max_height;
    width /= ratio;
    height /= ratio;
  }
  pixbuf = gdk_pixbuf_scale_simple(pixbuf, width, height, GDK_INTERP_BILINEAR);
  gtk_image_set_from_pixbuf(GTK_IMAGE(image), pixbuf);

  return image;
}

static GtkWidget* image_label_box (gchar* imagefile, gchar* labeltext, int max_width, int max_height)
{
    GtkWidget *box;
    GtkWidget *label;
    GtkWidget *image;

    /* Create box for image and label */
    box = gtk_vbox_new (FALSE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (box), 2);

    /* Now on to the image stuff */
    image = load_image(imagefile, max_width, max_height);

    /* Create a label for the button */
    label = gtk_label_new (labeltext);

    /* Pack the image and label into the box */
    gtk_box_pack_start (GTK_BOX (box), image, FALSE, FALSE, 3);
    gtk_box_pack_start (GTK_BOX (box), label, FALSE, FALSE, 3);

    gtk_widget_show (image);
    gtk_widget_show (label);

    return box;
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
  menu_elements *actions;
  const char* conffile = "/etc/gappman/shutdown.xml";
  int dialog_width;
  int dialog_height;
  int c;
  char* shutdownimagefile = "/usr/share/pixmaps/gappman/system-shutdown.png";
  char* restartimagefile = "/usr/share/pixmaps/gappman/system-restart.png";
  char* suspendimagefile = "/usr/share/pixmaps/gappman/system-suspend.png";

  gtk_init (&argc, &argv);

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

  /** Load configuration elements */
  loadConf(conffile);
  actions = getActions();
  
  screen = gdk_screen_get_default ();
  dialog_width =  gdk_screen_get_width (screen) / 9;
  dialog_height =  gdk_screen_get_height (screen) / 9;

  mainwin = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  
  //Make window transparent
  //  gtk_window_set_opacity (GTK_WINDOW (mainwin), 0.0);
  
  //Remove border
  gtk_window_set_decorated (GTK_WINDOW (mainwin), FALSE);

	vbox = gtk_vbox_new (FALSE, 10);

  if ( actions != NULL )
  {
    align = createbuttons( actions, gdk_screen_get_width (screen), gdk_screen_get_height (screen), &process_startprogram_event );
    gtk_container_add (GTK_CONTAINER (vbox), align);
    gtk_widget_show (align);
  }

	hbox = gtk_hbox_new (FALSE, 10); 
  // cancel button
  button = gtk_button_new_with_label("Cancel");
	g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (gtk_main_quit), NULL);
  gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);
  gtk_widget_show(button);

  gtk_container_add (GTK_CONTAINER (vbox), hbox);
	gtk_widget_show (hbox); 

	gtk_container_add (GTK_CONTAINER (mainwin), vbox);
	gtk_widget_show (vbox); 
  gtk_widget_show (mainwin);
  
  gtk_main ();

  freeMenuElements( actions );

  return 0;
}

