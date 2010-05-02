/***
 * \file gm_connect.h
 *
 *
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 */


/**
* \brief Connects to gappman and requests the fontsize
* \param portno portnumber gappman listens to
* \param hostname servername of host that runs gappman
* \param fontsize adres of the pointer to an integer value (call by reference)
* \return integer value as defined in libs/generic/gm_generic.h
*	GM_SUCCES
*	GM_COULD_NOT_RESOLVE_HOSTNAME
* GM_COULD_NOT_CONNECT
* GM_COULD_NOT_SEND_MESSAGE
* GM_COULD_NOT_DISCONNECT
*/
int gm_get_fontsize_from_gappman(int portno, const char* hostname, int *fontsize);

/**
* \brief Connects to gappman
* \param portno portnumber gappman listens to
* \param hostname servername of host that runs gappman
* \param sockfd pointer to int which will hold the socket filedescriptor
* \return filedescriptor
*/
int gm_connect_to_gappman(int portno, const char* hostname, int *sockfd);

/**
* \brief connects to gappman and sends a message and may receive one or more answers.
* \param portno portnumber gappman listens to
* \param hostname servername of host that runs gappman
* \param msg the message that should be sent to gappman
* \param callbackfunc callback function that should handle each message received from gappman. If NULL no messages will be received.
* \return integer value as defined in libs/generic/gm_generic.h
*/
int gm_send_and_receive_message(int portno, const char* hostname, gchar *msg, void (*callbackfunc)(gchar*));
