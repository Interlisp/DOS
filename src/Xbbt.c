/* @(#) Xbbt.c Version 1.6 (6/9/92). copyright venue & Fuji Xerox  */
static char *id = "@(#) Xbbt.c	1.6 6/9/92	(venue & Fuji Xerox)";
/*
*
*
* Copyright (C) 1988 by Fuji Xerox co.,Ltd. All rights reserved.
*
*		Author: Mitsunori Matsuda
*		Date  : August 11, 1988
*
*/


/************************************************************************/
/*									*/
/*	Copyright 1989, 1990 Venue, Fuji Xerox Co., Ltd, Xerox Corp.	*/
/*									*/
/*	This file is work-product resulting from the Xerox/Venue	*/
/*	Agreement dated 18-August-1989 for support of Medley.		*/
/*									*/
/************************************************************************/


#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "ldeXdefs.h"

#include "dspif.h"
extern DspInterface currentdsp;

/************************************************************************/
/*									*/
/*		    c l i p p i n g _ X b i t b l t			*/
/*									*/
/*	BITBLT from the display region to the X server's display,	*/
/*	clipping to fit the window we're in on the server.		*/
/*									*/
/*	dummy is the placeholder for the bitmap to be blitted		*/
/*									*/
/************************************************************************/
unsigned long
clipping_Xbitblt(dsp, dummy, x, y, w, h)
     DspInterface dsp;
     int dummy, x, y, w, h;
{ 
  int temp_x, temp_y, LowerRightX, LowerRightY;

  LowerRightX = dsp->UpperLeftX + dsp->VissibleWidth;
  LowerRightY = dsp->UpperLeftY + dsp->VissibleHeight;

  temp_x = x + w - 1; 
  temp_y = y + h - 1; 

  if ( (temp_x < dsp->UpperLeftX)
      || (x > LowerRightX) 
      || (temp_y < dsp->UpperLeftY) 
      || (y > LowerRightY)) return(0); 
	
  if ( ( x >= dsp->UpperLeftX ) 
      && ( temp_x <= LowerRightX ) 
      && ( y >= dsp->UpperLeftY ) 
      && ( temp_y <= LowerRightY ) )
    { 
      XLOCK;
      XPutImage( dsp->display_id,
		dsp->DisplayWindow,
		dsp->Copy_GC,
		dsp->ScreenBitmap,
		x , y, x - dsp->UpperLeftX,
		y - dsp->UpperLeftY, w, h );
      XFlush(dsp->display_id);
      XUNLOCK;
      return(0); 
    }
 
  if ( x < dsp->UpperLeftX )
    { 
      w -= dsp->UpperLeftX - x; 
      x = dsp->UpperLeftX; 
    }
 
  if ( temp_x > LowerRightX ) w -= temp_x - LowerRightX; 
 
  if ( y < dsp->UpperLeftY )
    { 
      h -= dsp->UpperLeftY - y; 
      y = dsp->UpperLeftY; 
    }
 
  if ( temp_y > LowerRightY ) h -= temp_y - LowerRightY; 
 
  if ((w>0) && (h>0))
    { 
      XLOCK;
      XPutImage( dsp->display_id,
		dsp->DisplayWindow,
		dsp->Copy_GC,
		dsp->ScreenBitmap,
		x , y, x - dsp->UpperLeftX,
		y - dsp->UpperLeftY, w, h );
      XFlush(dsp->display_id);
      XUNLOCK;
      return(0);
    }

}				/* end clipping_Xbitblt */
