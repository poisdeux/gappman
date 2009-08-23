#include <gtk/gtk.h>

/**
* \brief Struct that holds all relevant info about started applications
*/ 
struct appwidgetinfo
{
  int PID; //<! Process ID of running app (child replaced through execvp)
  int status; //<! Process status which can be either running, sleeping, waiting, stopped, or zombie
  GtkWidget *widget; //<! Button that started the process
  struct appwidgetinfo* prev; //<! Pointer to previous appwidgetinfo
  struct appwidgetinfo* next; //<! Pointer to previous appwidgetinfo
};


/**
* \brief Returns the started applications.
* \return pointer to the appwidgetinfo struct
*/

struct appwidgetinfo* get_started_apps();
