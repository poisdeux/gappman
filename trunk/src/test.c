/***
 * \file test.c
 *
 *
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 */


#include "changeresolution.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

void usage()
{
    printf("usage: test --changeresolution --width PIXELS --height PIXELS\n");
}

int main(int argc, char **argv)
{
    int width = -1;
    int height = -1;
    int c;
    int change_resolution = 0;

    gtk_init (&argc, &argv);

    while (1)
    {
        int option_index = 0;
        static struct option long_options[] = {
            {"changeresolution", 0, 0, 'r'},
            {0, 0, 0, 0}
        };

        c = getopt_long(argc, argv, "rw:h:",
                        long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
        case 'r':
            change_resolution = 1;
            break;
        case 'w':
            width=atoi(optarg);
            break;
        case 'h':
            height=atoi(optarg);
            break;
        default:
            usage();
            return 0;
        }
    }

    if ( change_resolution )
    {
        error_type = changeresolution( width, height);
        if ( error_type != SUCCES )
        {
            return 1;
        }
        return 0;
    }
}
