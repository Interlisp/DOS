/* @(#) DoScroll.c Version 1.5 (5/27/92). copyright venue & Fuji Xerox  */
static char *id = "@(#) DoScroll.c	1.5 5/27/92	(venue & Fuji Xerox)";
/*
*
*
* Copyright (C) 1988 by Fuji Xerox co.,Ltd. All rights reserved.
*
*		Author: Mitsunori Matsuda
*		Date  : August 11, 1988
*/

#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "XVersion.h"
#include "dspif.h"

#define SCROLL_PITCH      30

int ScrollPitch = SCROLL_PITCH;

JumpScrollVer( dsp, y )
     DspInterface dsp;
{
  Scroll( dsp, dsp->UpperLeftX, (int)((dsp->displaywidth *y) / dsp->VissibleHeight));
}

JumpScrollHor( dsp, x )
     DspInterface dsp;
{
  Scroll( dsp, (int)((dsp->displaywidth *x) / dsp->VissibleWidth), dsp->UpperLeftY);
}

ScrollLeft( dsp )
     DspInterface dsp;
{
  Scroll( dsp, dsp->UpperLeftX - ScrollPitch, dsp->UpperLeftY);
}

ScrollRight( dsp )
     DspInterface dsp;
{
  Scroll( dsp, dsp->UpperLeftX + ScrollPitch, dsp->UpperLeftY);
}

ScrollUp( dsp )
     DspInterface dsp;
{
  Scroll( dsp, dsp->UpperLeftX, dsp->UpperLeftY - ScrollPitch);
}

ScrollDown( dsp )
     DspInterface dsp;
{
  Scroll( dsp, dsp->UpperLeftX, dsp->UpperLeftY + ScrollPitch);
}


/* Move the DisplayWindow and the ScrollButtons to a new */
/* position. newX, newY refers to the uppre left corner */
/* of the LispDisplay */
Scroll( dsp, newX, newY)
     DspInterface dsp;
     int newX, newY;
{
  /* Limit the newX and newY values. */
  dsp->UpperLeftX = bound(0, newX, dsp->displaywidth - dsp->VissibleWidth);
  dsp->UpperLeftY = bound(0, newY, dsp->displayheight - dsp->VissibleHeight);

  newX = (int)((dsp->UpperLeftX * dsp->VissibleWidth) / dsp->displaywidth);
  newY = (int)((dsp->UpperLeftY * dsp->VissibleHeight) / dsp->displayheight);

  XMoveWindow( dsp->display_id, dsp->HorScrollButton, newX, -dsp->InternalBorderWidth);
  XMoveWindow( dsp->display_id, dsp->VerScrollButton, -dsp->InternalBorderWidth, newY );

  (dsp->bitblt_to_screen)( dsp, 0, dsp->UpperLeftX, dsp->UpperLeftY,
			  dsp->VissibleWidth, dsp->VissibleHeight);
}/* end Scroll */
