/* @(#) Xmkicon.c Version 1.4 (10/8/92). copyright venue & Fuji Xerox  */
static char *id = "@(#) Xmkicon.c	1.4 10/8/92	(venue & Fuji Xerox)";

/************************************************************************/
/*									*/
/*	Copyright 1989, 1990, 1991 Venue				*/
/*									*/
/*	This file was work-product resulting from the Xerox/Venue	*/
/*	Agreement dated 18-August-1989 for support of Medley.		*/
/*									*/
/************************************************************************/




#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "dbprint.h"

#include "XVersion.h"
#include "dspif.h"

XImage IconImage;

extern int Bitmap_Pad , Default_Depth, Lisp_icon_width, Lisp_icon_height;
extern char Lisp_icon[];

extern Pixmap IconPixmap;
extern iconpixmapfile;
/************************************************************************/
/*									*/
/*			m a k e _ X i c o n				*/
/*									*/
/*	Make the icon for the shrunken Medley window.			*/
/*									*/
/************************************************************************/

make_Xicon( dsp )
     DspInterface dsp;
  {
    unsigned int width, height;
    int value, x_hot, y_hot;
    char *getenv();
    
#ifdef TRACE 
    printf( "In make_Xicon().\n" );
#endif
    value = XReadBitmapFile(dsp->display_id,
			    RootWindow(dsp->display_id, 0),
			    &iconpixmapfile,
			    &width, &height,
			    &IconPixmap,
			    &x_hot, &y_hot);
    if  (value == BitmapOpenFailed) {
      IconImage.width = Lisp_icon_width;
      IconImage.height = Lisp_icon_height;
      IconImage.xoffset = 0;
      IconImage.format  = XYBitmap;
      IconImage.data    = (char*) Lisp_icon;
#if defined(BYTESWAP)
      IconImage.byte_order  = LSBFirst;
#else BYTESWAP
      IconImage.byte_order  = MSBFirst;
#endif BYTESWAP
      IconImage.bitmap_unit = 8;
      IconImage.bitmap_pad  = Bitmap_Pad;
      IconImage.depth       = 1;
      IconImage.bytes_per_line = Lisp_icon_width/8;
#if defined(X_ICON_IN_X_BITMAP_FORMAT)
      IconImage.bitmap_bit_order = LSBFirst;
#else
      IconImage.bitmap_bit_order = MSBFirst;
#endif

      IconPixmap = XCreatePixmap( dsp->display_id, dsp->LispWindow
				 , Lisp_icon_width , Lisp_icon_height
				 , DefaultDepthOfScreen( ScreenOfDisplay( dsp->display_id,
									 DefaultScreen( dsp->display_id ) ) ) );

      XPutImage( dsp->display_id, IconPixmap, dsp->Copy_GC, &IconImage
		, 0, 0, 0, 0
		, Lisp_icon_width, Lisp_icon_height );

    }
    else if (value == BitmapFileInvalid)
      fprintf(stderr, "Iconpixmapfile %s contains invalid bitmap data\n", iconpixmapfile);
    else if (value == BitmapNoMemory)
      fprintf(stderr, "Not enough memory to allocate icon pixmap\n");
    return( IconPixmap );
  }				/* end make_Xicon */
