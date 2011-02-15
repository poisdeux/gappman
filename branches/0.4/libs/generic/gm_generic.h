/**
 * \file gm_generic.h
 * \brief generic stuff that is generally used by all gappman code
 *
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 */
#ifndef __GAPPMAN_GENERIC_H__
#define __GAPPMAN_GENERIC_H__

#ifndef SYSCONFDIR
#define SYSCONFDIR "/etc/gappman" ///< default location for the configuration files
#endif

#define GM_SUCCES 0	///< no errors detected
#define GM_FAIL 9 ///< used to represent a general error
#define GM_NO_RANDR_EXTENSION 1 ///< Xorg has no support for the XRANDR extension. This is fatal for gm_changeresolution
#define GM_NO_SCREEN_CONFIGURATION 2 ///< No screen configuration could be retrieved. This is fatal for gm_changeresolution
#define GM_SIZE_NOT_AVAILABLE 3 ///< No screen sizes found that match a requested resolution. This should never happen and is only possible if wrong values are passed. 
#define GM_COULD_NOT_RESOLVE_HOSTNAME 4 ///< Returned by gm_connect lib when hostname could not be resolved to an ip-address
#define GM_COULD_NOT_CONNECT 5 ///< Returned by gm_connect lib when connection with gappman failed
#define GM_COULD_NOT_SEND_MESSAGE 6 ///< Returned by gm_connect lib when sending message failed
#define GM_COULD_NOT_DISCONNECT 7 ///< Returned by gm_connect lib could not disconnect connection with gappman
#define GM_COULD_NOT_LOAD_FILE 8 ///< Could not open or read a file
#define GM_NET_COMM_NOT_SUPPORTED 9 ///< No support for network-communications

#endif
