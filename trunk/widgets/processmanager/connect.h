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
* \param starteprocs adres of the pointer to a proceslist structure (call by reference)
* \return integer value
*			0: OK
*			1: Could not resolve hostname
*			2: Could not connect
*			3: Could not send message
*			4: Could not shutdown channel/disconnect
*/
int getProceslistFromGappman(int portno, const char* hostname, struct proceslist **startedprocs);

/**
* \brief Frees the proceslist structure
* \param procslist pointer to the last element of the proceslist linked list
*/
void freeproceslist(struct proceslist* procslist);
