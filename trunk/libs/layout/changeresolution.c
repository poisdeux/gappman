#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <stdlib.h>
#include "changeresolution.h"


int gm_getpossibleresolutions (XRRScreenSize **sizes, int *nsize)
{
  GdkDisplay *gdk_dpy;
	int major;
	int minor;
  XRRScreenConfiguration *sc;

	gdk_dpy = gdk_display_get_default ();
  
  sc = XRRGetScreenInfo ( GDK_DISPLAY_XDISPLAY(gdk_dpy), GDK_ROOT_WINDOW() );
  
  if (!XRRQueryVersion (GDK_DISPLAY_XDISPLAY(gdk_dpy), &major, &minor))
  {
	 return NO_RANDR_EXTENSION;
  }
  
  if (sc == NULL) 
	 return NO_SCREEN_CONFIGURATION;
	
  *sizes = XRRConfigSizes(sc, nsize);

	return SUCCES;
}

XRRScreenSize gm_getcurrentsize()
{
	XRRScreenSize *sizes;
	int nr;
	gm_getpossibleresolutions(&sizes, &nr);
	return sizes[0];
}

int gm_changeresolution (int width, int height)
{
  XRRScreenSize *sizes;
  XRRScreenConfiguration *sc;
  Rotation current_rotation;
  short current_rate;
  SizeID	current_size;
  short *rates;
  short min_rate_dist;
  int nrate;
  int size = -1;
  int nsize;
  int min_size_dist;
  int major;
  int minor;
  short rate = -1;
  int i;
  GdkDisplay *gdk_dpy;

	printf("changeresolution: %d x %d\n",width, height);  
  gdk_dpy = gdk_display_get_default ();
  
  
  sc = XRRGetScreenInfo ( GDK_DISPLAY_XDISPLAY(gdk_dpy), GDK_ROOT_WINDOW() );
  
  if (!XRRQueryVersion (GDK_DISPLAY_XDISPLAY(gdk_dpy), &major, &minor))
  {
	 return NO_RANDR_EXTENSION;
  }
  
  if (sc == NULL) 
	 return NO_SCREEN_CONFIGURATION;
	
	current_size = XRRConfigCurrentConfiguration (sc, &current_rotation);

  sizes = XRRConfigSizes(sc, &nsize);
  
  // Search wanted resolution in available resolutions
  // Take resolution that best matches wanted resolution
  // Higher resolutions should prevail over lower resolutions
  // so we take < instead of <=.
  min_size_dist = width + height;
  for (i = 0; i < nsize; i++)
	{
		  if ( sizes[i].width == width && sizes[i].height == height )
		  {
		    size = i;
		    break;
		  } else if ( abs((sizes[i].width + sizes[i].height) - (width + height)) < min_size_dist )
		  {
		    min_size_dist = abs((sizes[i].width + sizes[i].height) - (width + height));
		    size = i;
		  }
	}
	if (size >= nsize) {
	    fprintf (stderr,
		     "Size %dx%d not found in available modes not changing resolution\n", width, height);
	    return error_type = SIZE_NOT_AVAILABLE;
	}
	
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
        rate = rates[i];
      }
    }
  }
  else
  {
    rate = current_rate;
  }
  
  //Change resolution if needed
  if ( size != current_size )
  {
    fprintf(stdout, "Changing size to %dx%d, and rate to %-4d\n", sizes[size].width, sizes[size].height, rate);
    //Grab X server while we mess with it.
    gdk_x11_display_grab ( gdk_dpy );

    XRRSelectInput ( GDK_DISPLAY_XDISPLAY(gdk_dpy), GDK_ROOT_WINDOW(), RRScreenChangeNotifyMask);
    XRRSetScreenConfigAndRate ( GDK_DISPLAY_XDISPLAY(gdk_dpy), sc, GDK_ROOT_WINDOW(), size, current_rotation, rate, CurrentTime);
    XRRFreeScreenConfigInfo(sc);
    
    //Release X server
		gdk_x11_display_ungrab ( gdk_dpy );
	}
	
	return SUCCES;
}
