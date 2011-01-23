/**
 * \file gm_connect-generic.c
 * \brief generic functions used by gm_connect-*.c
 *
 *
 * GPL v2
 *
 * \author
 *   Martijn Brekhof <m.brekhof@gmail.com>
 *
*/

#include "gm_connect-generic.h"
#include <glib.h>

struct proceslist* createnewproceslist(struct proceslist* procs)
{
    struct proceslist* newproc;
    newproc = (struct proceslist*) g_try_malloc0(sizeof(struct proceslist));
    if ( newproc != NULL)
    {
        newproc->name = "";
        newproc->pid = -1;
        newproc->prev = procs;
    }
    return newproc;
}

