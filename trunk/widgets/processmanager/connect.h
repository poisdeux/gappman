#include <gtk/gtk.h>

/**
* \brief struct to hold the process ID and program name retrieved from Gappman
*/
struct proceslist {
  int pid;
  gchar* name;
	struct proceslist* prev;
};

/**
* \brief Connects to gappman and requests the proceslist
* \param portno portnumber gappman listens to
* \param server servername of host that runs gappman
*/
struct proceslist* getProceslistFromGappman(int portno, const char* hostname);

/**
* \brief Frees the proceslist structure
* \param procslist pointer to the last element of the proceslist linked list
*/
void freeproceslist(struct proceslist* procslist);
