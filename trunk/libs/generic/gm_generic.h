/***
 * \file gm_generic.h
 *
 *
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 */


#ifndef __GAPPMAN_GENERIC_H__
#define __GAPPMAN_GENERIC_H__

/**
* \enum gm_return_values
* Enumeration for generic return values as used by the gappman lib
*/
#define GM_SUCCES 0	///< no errors detected
#define GM_NO_RANDR_EXTENSION 1 ///< Xorg has no support for the XRANDR extension. This is fatal for gm_changeresolution
#define GM_NO_SCREEN_CONFIGURATION  ///< No screen configuration could be retrieved. This is fatal for gm_changeresolution
#define GM_SIZE_NOT_AVAILABLE 3 ///< No screen sizes found that match a requested resolution. This should never happen and is only possible if wrong values are passed. 
#define GM_COULD_NOT_RESOLVE_HOSTNAME 4 
#define GM_COULD_NOT_CONNECT 5
#define GM_COULD_NOT_SEND_MESSAGE 6
#define GM_COULD_NOT_DISCONNECT 7
#define GM_COULD_NOT_LOAD_FILE 8

#endif
