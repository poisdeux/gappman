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
* \return filedescriptor
*/
int gm_connect_to_gappman(int portno, const char* hostname);
