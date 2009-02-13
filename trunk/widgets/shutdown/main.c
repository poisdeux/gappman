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
   dialog_width =  gdk_screen_get_width (screen) / 9;
   dialog_height =  gdk_screen_get_height (screen) / 9;

  mainwin = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  
  //Make window transparent
  //  gtk_window_set_opacity (GTK_WINDOW (mainwin), 0.0);
  
  //Remove border
  gtk_window_set_decorated (GTK_WINDOW (mainwin), FALSE);
  gtk_window_set_default_size (GTK_WINDOW (mainwin), dialog_width, dialog_height);

  // shutdown, reboot, suspend
  table = gtk_table_new(2, 3, TRUE); 

  // shutdown button
  button = gtk_button_new();
  labelimagebox = image_label_box("/usr/share/pixmaps/gappman/system-shutdown.png", "Shutdown", dialog_width, dialog_height); 
  gtk_container_add (GTK_CONTAINER (button), labelimagebox);
  gtk_widget_show (labelimagebox);
  gtk_widget_show(button);
  gtk_table_attach_defaults(GTK_TABLE(table), button, 0, 1, 0, 1); 
  
  // reboot button
  button = gtk_button_new();
  labelimagebox = image_label_box("/usr/share/pixmaps/gappman/system-restart.png", "Reboot", dialog_width, dialog_height); 
  gtk_container_add (GTK_CONTAINER (button), labelimagebox);
  gtk_widget_show (labelimagebox);
  gtk_widget_show(button);
  gtk_table_attach_defaults(GTK_TABLE(table), button, 1, 2, 0, 1); 

  // suspend button
  button = gtk_button_new();
  labelimagebox = image_label_box("/usr/share/pixmaps/gappman/system-suspend.png", "Suspend", dialog_width, dialog_height); 
  gtk_container_add (GTK_CONTAINER (button), labelimagebox);
  gtk_widget_show (labelimagebox);
  gtk_widget_show(button);
  gtk_table_attach_defaults(GTK_TABLE(table), button, 2, 3, 0, 1); 
 
  // cancel button
  button = gtk_button_new_with_label("Cancel");
  gtk_widget_show(button);
  gtk_table_attach_defaults(GTK_TABLE(table), button, 1, 3, 1, 2); 
  
  gtk_container_add (GTK_CONTAINER (mainwin), table);
  gtk_widget_show (table);
  gtk_widget_show (mainwin);
  
  gtk_main ();

  return 0;
}

