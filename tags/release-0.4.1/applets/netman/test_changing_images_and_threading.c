#include <gtk/gtk.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

// gcc test_changing_images_and_threading.c -o test `pkg-config --libs --cflags gtk+-2.0 gthread-2.0`

static GtkWidget* button;

/** \struct images
* \brief list of images.
*/ 
struct images {
	GtkWidget* image; ///< an image
	struct images *next; ///<  pointer to next images struct
};

struct images *list_of_images;

static GMutex *mutex = NULL;

static gint exec_program()
{
    int status = -1;
    int ret;
    int i;
    char *args[4];
    __pid_t childpid;
    FILE *fp;

            args[0] = "/usr/bin/ping";
					  args[1] = "-c 1";
					  args[2] = "google.com";
            args[3] = NULL;
			system("/bin/ping -c 1 google.com");
//	  g_spawn_sync(NULL, args, NULL, G_SPAWN_STDOUT_TO_DEV_NULL | G_SPAWN_STDERR_TO_DEV_NULL, 
//			NULL, NULL, NULL, NULL, &status, NULL);
/*        childpid = fork();
        if ( childpid == 0 )
        {
            /**
            Create argument list. First element should be the filename
            of the executable and last element needs to be NULL.
            see man exec for more details
            */
        /*    args[0] = "/usr/bin/ping";
					  args[1] = "-c 1";
					  args[2] = "google.com";
            args[3] = NULL;

            execvp("/usr/bin/ping", args);
            _exit(0);
        }
        wait(&status);
*/
    return WEXITSTATUS(status);
}

static void set_image(GtkWidget *lbutton, GtkWidget* image)
{
	GtkWidget* current_image;
  current_image = gtk_bin_get_child(GTK_BIN(button));
  gtk_container_remove(GTK_CONTAINER(button), current_image);

	gtk_container_add(GTK_CONTAINER(button), image);
}
 
GThreadFunc start_thread()
{
	struct images *image_iter;
	int count = 0;
	while(1)
	{
		g_mutex_lock(mutex);
		image_iter = list_of_images;
		while(image_iter != NULL)
		{
			exec_program();
			set_image(button, image_iter->image);
			sleep(1);
			g_debug("Round %d", count++);
			image_iter = image_iter->next;
		}
		g_mutex_unlock(mutex);
	}
}

static struct images* create_image_struct(struct images *next)
{
	struct images *new_image;
	new_image = (struct images*) malloc(sizeof(struct images));
	new_image->next = next;
	return new_image;
}

int main(int argc, char **argv)
{
  GtkWidget *mainwin;
	struct images* image_elt;

	g_thread_init (NULL);
  gdk_threads_init();
	gtk_init (&argc, &argv);

	g_assert(mutex == NULL);	
	mutex = g_mutex_new();

	mainwin = gtk_window_new (GTK_WINDOW_TOPLEVEL);

	image_elt = create_image_struct(NULL);
	image_elt->image = gtk_image_new_from_file("/usr/share/pixmaps/apple-green.png");
	gtk_widget_show(image_elt->image);
	g_object_ref(image_elt->image);

	list_of_images = image_elt;

	image_elt = create_image_struct(list_of_images);
	image_elt->image = gtk_image_new_from_file("/usr/share/pixmaps/apple-red.png");
	gtk_widget_show(image_elt->image);
	g_object_ref(image_elt->image);

	list_of_images = image_elt;

	button = gtk_button_new();	
  gtk_container_add(GTK_CONTAINER(button), image_elt->image);
	gtk_widget_show(button);
	
	gtk_container_add(GTK_CONTAINER(mainwin), button);
	gtk_widget_show(mainwin);

	if (!g_thread_create((GThreadFunc) start_thread, NULL, FALSE, NULL))
  {
  	g_error("Failed to create thread");
  }

	gdk_threads_enter();
  gtk_main ();
  gdk_threads_leave();
}
