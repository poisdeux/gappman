/**
 * \file gm_changeresolution.h
 * \brief Changes X screen resolution using the Xrandr extension
 *
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 */
#ifndef __GAPPMAN_CHANGERESOLUTION_H__
#define __GAPPMAN_CHANGERESOLUTION_H__

#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>


/**
* \brief tries to change the display resolution to width x height.
* \param width new display width
* \param height new display height
* \return gm_error
*/
int gm_changeresolution (int width, int height);

/**
* \brief Queries using Xrandr the possible screen resolutions
* \param **sizes pointer to a list of XRRScreenSizes
* \param *size pointer to integer that will hold the amount of available sizes
* \return int Error type, see enum error_types
*/
int gm_getpossibleresolutions (XRRScreenSize **sizes, int *size);

/**
* \brief returns the current screen resolution
* \return XRRScreenSize* pointer to a XRRScreenSize struct that will hold the resolution
*/
int gm_get_current_size(XRRScreenSize *size);

#endif
