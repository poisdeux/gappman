/**
 * \file connect.h
 *
 *
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 */


#include <gtk/gtk.h>

/**
* \brief struct to hold the process ID and program name retrieved from Gappman
*/
struct proceslist {
    int pid;	///< process ID of proces started by gappman
    gchar* name; ///< programname as known by gappman
    struct proceslist* prev; ///< pointer to next proces in proceslist
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
int getStartedProcsFromGappman(int portno, const char* hostname, struct proceslist **startedprocs);

/**
* \brief Connects to gappman and requests the fontsize
* \param portno portnumber gappman listens to
* \param server servername of host that runs gappman
* \param fontsize adres of the pointer to an integer value (call by reference)
* \return integer value
*			0: OK
*			1: Could not resolve hostname
*			2: Could not connect
*			3: Could not send message
*			4: Could not shutdown channel/disconnect
*/
int getFontsizeFromGappman(int portno, const char* hostname, int *fontsize);

/**
* \brief Frees the proceslist structure
* \param procslist pointer to the last element of the proceslist linked list
*/
void freeproceslist(struct proceslist* procslist);
