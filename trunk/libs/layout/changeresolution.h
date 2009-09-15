/**
 * \file changeresolution.h
 * \brief Changes X screen resolution using the Xrandr extension
 * 
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 */
#include <X11/extensions/Xrandr.h>

/**
* Enumeration for error types.
* Currently supported types are: 
* SUCCES: no error
* NO_RANDR_EXTENSION: X server has no support for the RANDR extension
* NO_SCREEN_CONFIGURATION: could not get a screen configuration for the current display. This means that the xrandr extension is not available for the current screen.
* SIZE_NOT_AVAILABLE: provided width x height is not available
* RATE_NOT_AVAILABLE: could not determine a proper rate for the new screen size
*/
enum error_types {
  SUCCES,
  NO_RANDR_EXTENSION,
  NO_SCREEN_CONFIGURATION,
  SIZE_NOT_AVAILABLE,
  RATE_NOT_AVAILABLE
} error_type;

/**
* \brief tries to change the display resolution to width x height.
* \param width new display width
* \param height new display height
*/
int changeresolution (int width, int height);

/**
* \brief Queries using Xrandr the possible screen resolutions
* \param **sizes pointer to a list of XRRScreenSizes
* \return int Error type, see enum error_types 
*/
int getpossibleresolutions (XRRScreenSize **sizes);
