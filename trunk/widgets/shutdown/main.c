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

static void usage()
{
	printf("./shutdown\n");
}

static GtkWidget* createbutton(char* imagefile, int max_width, int max_height)
{
  GtkWidget *button;
  GtkWidget *image;
  GdkPixbuf *pixbuf;
  int width,height;
  double ratio;

  button = gtk_button_new();

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
    gtk_button_set_image(GTK_BUTTON(button), image);

    return button;
}

/**
* \brief main function setting up the UI
*/
int main (int argc, char **argv)
{
  GdkScreen *screen;
  GdkWindow *rootwin;
  GdkPixbuf *pixbuf;
  GtkWidget *button;
  GtkWidget *image;
  GtkWidget *mainwin;
  GtkWidget *table;
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
          {"help", 1, 0, 'h'},
          {0, 0, 0, 0}
      };

      c = getopt_long(argc, argv, "h",
              long_options, &option_index);
      if (c == -1)
          break;

      switch (c) {
      case 'h':
          usage();
          return 0;
      default:
	;;
      }
  }

  /** Load configuration elements */
  //loadConf(conffile);

   screen = gdk_screen_get_default ();
   dialog_width =  gdk_screen_get_width (screen) / 4;
   dialog_height =  gdk_screen_get_height (screen) / 4;

  mainwin = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  
  //Make window transparent
  //  gtk_window_set_opacity (GTK_WINDOW (mainwin), 0.0);
  
  //Remove border
  gtk_window_set_decorated (GTK_WINDOW (mainwin), FALSE);
  gtk_window_set_default_size (GTK_WINDOW (mainwin), dialog_width, dialog_height);

  // shutdown, reboot, suspend
  table = gtk_table_new(1, 3, TRUE); 

  // shutdown button
  button = createbutton(shutdownimagefile, dialog_width, dialog_height); 
  gtk_table_attach_defaults(GTK_TABLE(table), button, 0, 1, 0, 1); 
  gtk_widget_show(button);
  
  // reboot button
  button = createbutton(restartimagefile, dialog_width, dialog_height); 
  gtk_table_attach_defaults(GTK_TABLE(table), button, 1, 2, 0, 1); 
  gtk_widget_show(button);

  // suspend button
  button = createbutton(suspendimagefile, dialog_width, dialog_height); 
  gtk_table_attach_defaults(GTK_TABLE(table), button, 2, 3, 0, 1); 
  gtk_widget_show(button);
  
  gtk_container_add (GTK_CONTAINER (mainwin), table);
  gtk_widget_show (table);
  gtk_widget_show (mainwin);
  
  gtk_main ();

  return 0;
}

