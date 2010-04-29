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
* Enumeration for error types.
* Currently supported types are: 
* SUCCES: no error
* NO_RANDR_EXTENSION: X server has no support for the RANDR extension
* NO_SCREEN_CONFIGURATION: could not get a screen configuration for the current display. This means that the xrandr extension is not available for the current screen.
* SIZE_NOT_AVAILABLE: provided width x height is not available
* RATE_NOT_AVAILABLE: could not determine a proper rate for the new screen size
*/
#define GM_SUCCES 0
#define GM_NO_RANDR_EXTENSION 1
#define GM_NO_SCREEN_CONFIGURATION 2
#define GM_SIZE_NOT_AVAILABLE 3
#define GM_RATE_NOT_AVAILABLE 4
#define GM_COULD_NOT_RESOLVE_HOSTNAME 5
#define GM_COULD_NOT_CONNECT 6
#define GM_COULD_NOT_SEND_MESSAGE 7
#define GM_COULD_NOT_DISCONNECT 8

#endif
