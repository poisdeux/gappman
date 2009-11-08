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

