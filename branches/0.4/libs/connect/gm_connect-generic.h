/**
 * \file gm_connect-generic.h
 * \brief generic functions used by gm_connect-*.c
 *
 *
 * GPL v2
 *
 * \author
 *   Martijn Brekhof <m.brekhof@gmail.com>
 *
*/

#ifndef __GAPPMAN_CONNECT_GENERIC_H__
#define __GAPPMAN_CONNECT_GENERIC_H__

#include "gm_connect.h"

/**
* \brief creates a new proceslist struct and links it to the list procs
* \param procs list of proceslist structs. If this is the first this should be NULL
* \return pointer to the last proceslist struct
*/
struct proceslist* createnewproceslist(struct proceslist* procs);

#endif