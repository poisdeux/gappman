/**
 * \file gm_res_changeresolution.c
 *
 *
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 */


#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <stdlib.h>
#include <gm_changeresolution.h>
#include <gm_generic.h>

static XRRScreenConfiguration *sc = NULL;

void gm_res_free()
{
	XRRFreeScreenConfigInfo(sc);
}

int gm_res_init()
{
  GdkDisplay *gdk_dpy;

  gdk_dpy = gdk_display_get_default ();

	if (sc == NULL)
	{	
		sc = XRRGetScreenInfo ( GDK_DISPLAY_XDISPLAY(gdk_dpy), GDK_ROOT_WINDOW() );
		return GM_SUCCES;
	}
	else
	{
		g_warning("gm_res already initialized");
		return GM_FAIL;
	}
}

int gm_res_getpossibleresolutions (XRRScreenSize **sizes, int *nsize)
{
    if (sc == NULL)
    {
	    return GM_NO_SCREEN_CONFIGURATION;
		}
    *sizes = XRRConfigSizes(sc, nsize);

    return GM_SUCCES;
}

static int get_nearest_rate(XRRScreenConfiguration *sc, int size, short *rate)
{
    short current_rate;
    short *rates;
    short min_rate_dist;
    int nrate;
    int i;

    // Get current_rate and check if current rate is supported
    // for the new size
    current_rate = XRRConfigCurrentRate (sc);
    min_rate_dist = current_rate;
    rates = XRRConfigRates (sc, size, &nrate);
    // If there are no rates available use current rate
    if ( nrate > 0 )
    {
        for (i = 0; i < nrate; i++)
        {
            // We take smaller or equal as this will take the
            // largest rate if the current rate is exactly in
            // the middle of two available rates.
            if ( abs(current_rate - rates[i]) <= min_rate_dist )
            {
                min_rate_dist = abs(current_rate - rates[i]);
                *rate = rates[i];
            }
        }
    }
    else
    {
        *rate = current_rate;
    }

    return GM_SUCCES;
}

static int get_nearest_size(XRRScreenConfiguration *sc, int *size, int width, int height)
{
    int nsize;
    int min_size_dist, size_dist;
    int i;
    XRRScreenSize *sizes;

    sizes = XRRConfigSizes(sc, &nsize);

		//Initialize min_size_dist to the first available resolution from XRANDR
    min_size_dist = abs((sizes[0].width + sizes[0].height) - (width + height));

    for (i = 0; i < nsize; i++)
    {
        if ( sizes[i].width == width && sizes[i].height == height )
        {
            *size = i;
            break;
        } 
				else 
				{
					size_dist = abs((sizes[i].width + sizes[i].height) - (width + height));

    			// Higher resolutions should prevail over lower resolutions.
    			// So if two consecutive sizes result in equal distances with 
					// the requested resolution we take the first size.
					if ( size_dist < min_size_dist )
					{
            min_size_dist = size_dist;
            *size = i;
					}
        }
    }
    if (*size >= nsize) {
        g_warning("Size %dx%d not found in available modes not changing resolution\n", width, height);
        return GM_SIZE_NOT_AVAILABLE;
    }


    return GM_SUCCES;
}

int gm_res_changeresolution (int width, int height)
{
    Rotation current_rotation;
    SizeID	current_size;
    int size = -1;
    int ret_status;
    short rate = -1;
    GdkDisplay *gdk_dpy;

    gdk_dpy = gdk_display_get_default ();

    if (sc == NULL)
        return GM_NO_SCREEN_CONFIGURATION;

    current_size = XRRConfigCurrentConfiguration (sc, &current_rotation);

    ret_status = get_nearest_size(sc, &size, width, height);
    if ( ret_status != GM_SUCCES)
    {
        return ret_status;
    }


    get_nearest_rate(sc, size, &rate);

    //Change resolution if needed
    if ( size != current_size )
    {
        //Grab X server while we mess with it.
        gdk_x11_display_grab ( gdk_dpy );
        XRRSelectInput ( GDK_DISPLAY_XDISPLAY(gdk_dpy), GDK_ROOT_WINDOW(), RRScreenChangeNotifyMask);
        XRRSetScreenConfigAndRate ( GDK_DISPLAY_XDISPLAY(gdk_dpy), sc, GDK_ROOT_WINDOW(), size, current_rotation, rate, CurrentTime);

        //Release X server
        gdk_x11_display_ungrab ( gdk_dpy );
    }


    return GM_SUCCES;
}

int gm_res_get_current_size(XRRScreenSize* size)
{
    int nsize;
    SizeID size_id;
    Rotation current_rotation;
    GdkDisplay *gdk_dpy;
    XRRScreenSize *sizes;

    gdk_dpy = gdk_display_get_default ();

    if (sc == NULL)
    {
		g_warning("sc is NULL");
	    return GM_NO_SCREEN_CONFIGURATION;
	}
    sizes = XRRConfigSizes(sc, &nsize);

    size_id = XRRConfigCurrentConfiguration (sc, &current_rotation);

    *size = sizes[size_id];

    return GM_SUCCES;
}
