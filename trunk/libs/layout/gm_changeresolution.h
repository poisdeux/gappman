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
* \brief initializes the gm_res module. 
* This needs to be called before any of the other gm_res_* functions are called.
*/
int gm_res_init();

/**
* \brief releases the memory claimed when using the gm_res_* functions
*/
void gm_res_free();

/**
* \brief tries to change the display resolution to width x height.
* \param width new display width
* \param height new display height
* \return gm_res_error
*/
int gm_res_changeresolution(int width, int height);

/**
* \brief Queries using Xrandr the possible screen resolutions
* \param sizes pointer to a list of XRRScreenSizes
* \param size pointer to integer that will hold the amount of available sizes
* \return int Error type, see enum error_types
*/
int gm_res_getpossibleresolutions(XRRScreenSize ** sizes, int *size);

/**
* \brief returns the current screen resolution
* \return XRRScreenSize* pointer to a XRRScreenSize struct that will hold the resolution
*/
XRRScreenSize * gm_res_get_current_size();

#endif
