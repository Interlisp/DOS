/* @(#) XCursor.c Version 1.4 (9/18/90). copyright venue & Fuji Xerox  */
static char *id = "@(#) XCursor.c	1.4 9/18/90	(venue & Fuji Xerox)";



/************************************************************************/
/*									*/
/*	Copyright 1989, 1990 Venue, Fuji Xerox Co., Ltd, Xerox Corp.	*/
/*									*/
/*	This file is work-product resulting from the Xerox/Venue	*/
/*	Agreement dated 18-August-1989 for support of Medley.		*/
/*									*/
/************************************************************************/

/*
*
*
* Copyright (C) 1988 by Fuji Xerox co.,Ltd. All rights reserved.
*
*		Author: Mitsunori Matsuda
*		Date  : August 12, 1988
*/

#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "lispemul.h"
#include "iopage.h"
#include "display.h"
#include "dbprint.h"
#include "ldeXdefs.h"

#include "XVersion.h"

#include "dspif.h"

extern IOPAGE   *IOPage;

extern XGCValues gcv;
extern int Lisp_Xinitialized
	 , Bitmap_Pad
	 , Default_Depth;

XImage CursorImage;
Pixmap CursorPixmap_source
     , CursorPixmap_mask;
GC cursor_source_gc
 , cursor_mask_gc;
XColor cursor_fore_xcsd
     , cursor_back_xcsd
     , xced;
extern Colormap Colors;

extern DspInterface currentdsp;
/* a simple linked list to remember X cursors */
struct MXCURSOR {
  struct MXCURSOR *next;
  DLword bitmap[CURSORHEIGHT];
  Cursor Xid;
} *cursorlist = NULL;

/*
Cursor LispCursor[2];
int    cursor_sw;
*/


/************************************************************************/
/*									*/
/*			I n i t _ X C u r s o r				*/
/*									*/
/*	Initial setup for X cursor handling--create an initial		*/
/*	cursor, and get it displayed.					*/
/*									*/
/************************************************************************/

Init_XCursor()
{
  int i;
  DLword *newbm = (DLword *) (IOPage->dlcursorbitmap);

  TPRINT(( "TRACE: Init_DisplayCursor()\n" ));
  /* this is guaranteed to be our first cursor, isn't it? */
  cursorlist = (struct MXCURSOR *) malloc(sizeof(struct MXCURSOR));
  cursorlist->next = NULL;
  for(i=0; i<CURSORHEIGHT; i++)
    cursorlist->bitmap[i] = newbm[i];
  set_Xcursor( currentdsp, newbm, 0, 0, &(cursorlist->Xid), 1);
  DefineCursor(currentdsp->display_id,
	       currentdsp->DisplayWindow,
	       &(cursorlist->Xid));
}				/* end Init_XCursor */






/************************************************************************/
/*									*/
/*			S e t _ X C u r s o r				*/
/*									*/
/*	Set the X cursor from the Lisp bitmap, and move it to (x, y).	*/
/*									*/
/************************************************************************/

Set_XCursor( x, y )
  int x, y;
{
  /*
    Cursor *old_cursor;
    */
  /* compare cursor in IOPage memory with cursors we've seen before */
  register struct MXCURSOR *clp, *clbp;
  register DLword *newbm = ((DLword *) (IOPage->dlcursorbitmap));
  register int i;

  XLOCK;			/* No signals while setting the cursor */
  for(clp = cursorlist; clp != NULL; clbp = clp, clp = clp->next) {
    for(i=0; i< CURSORHEIGHT; i++)
      if(clp->bitmap[i] != newbm[i]) break;
    if(i == CURSORHEIGHT) break;
  }

  if(clp == NULL) {		/* it isn't there, push on a new one */
    clp = (struct MXCURSOR *) malloc(sizeof (struct MXCURSOR));
    /* and fill it up with the current new cursor */
    for(i=0; i< CURSORHEIGHT; i++)
      clp->bitmap[i] = newbm[i];
    set_Xcursor( currentdsp, newbm, 0, 0, &(clp->Xid), 1);
    clp->next = cursorlist;
    cursorlist = clp;
  }
  else
    /* found it, move it to the front of the list
       (this should reduce search time on the average by keeping
       the popular cursors near the front of the list)
       */
    if(clp != cursorlist) {	/* don't move if it's already there */
      clbp->next = clp->next;
      clp->next = cursorlist;
      cursorlist = clp;
    }
  DefineCursor(currentdsp->display_id,
	       currentdsp->DisplayWindow,
	       &(clp->Xid));
  XUNLOCK;			/* Signals OK now */

}				/* end Set_XCursor */




/************************************************************************/
/*									*/
/*			i n i t _ X c u r s o r				*/
/*									*/
/*	Initialization code for X-windows cursors.			*/
/*									*/
/************************************************************************/

init_Xcursor(display, window)
     Display *display;
{

    TPRINT(( "TRACE: init_Xcursor()\n" ));

	XLOCK;	/* Take no X signals during this activity (ISC 386) */

    CursorImage.width   = CURSORWIDTH;
    CursorImage.height  = CURSORHEIGHT;
    CursorImage.xoffset = 0;
    CursorImage.format  = XYBitmap;
#if (defined (XV11R1) || defined(BYTESWAP))
    CursorImage.byte_order  = LSBFirst;
#else XV11R1 | BYTESWAP
    CursorImage.byte_order  = MSBFirst;
#endif XV11R1 | BYTESWAP

    CursorImage.bitmap_unit = BITSPER_DLWORD;
#ifdef AIX
    CursorImage.bitmap_pad  = 32;
#else
    CursorImage.bitmap_pad  = Bitmap_Pad;
#endif AIX
    CursorImage.depth       = 1;
    CursorImage.bytes_per_line = BITSPER_DLWORD/8; 
    CursorImage.bitmap_bit_order = MSBFirst;

    CursorPixmap_source = XCreatePixmap( display, window , CURSORWIDTH, CURSORHEIGHT, 1);
    CursorPixmap_mask   = XCreatePixmap( display, window , CURSORWIDTH, CURSORHEIGHT, 1);

    gcv.function =  GXcopy;
    gcv.foreground = BlackPixelOfScreen( ScreenOfDisplay( display, DefaultScreen(display)));
    gcv.background = WhitePixelOfScreen( ScreenOfDisplay( display, DefaultScreen(display)));
#ifdef AIX
    gcv.plane_mask = 1;
#endif AIX

    cursor_source_gc = XCreateGC( display, window
				, GCForeground|GCBackground|GCFunction
#ifdef AIX
					|GCPlaneMask
#endif AIX
				, &gcv );
    cursor_mask_gc   = XCreateGC( display, window
				, GCForeground|GCBackground|GCFunction
#ifdef AIX
					|GCPlaneMask
#endif AIX
				, &gcv );

    XAllocNamedColor( display, Colors, "black" 
			, &cursor_fore_xcsd, &xced );
    XAllocNamedColor( display, Colors, "white" 
			, &cursor_back_xcsd, &xced );

	XUNLOCK;	/* OK to take signals again */

  } /* end init_Xcursor */



/************************************************************************/
/*									*/
/*			s e t _ X c u r s o r				*/
/*									*/
/*									*/
/*									*/
/************************************************************************/

set_Xcursor( dsp, bitmap, hotspot_x, hotspot_y, return_cursor, from_lisp )
     DspInterface dsp;
     unsigned char *bitmap;
     int hotspot_x
       , hotspot_y
       , from_lisp;
     Cursor *return_cursor;
{
  extern unsigned char reversedbits[];
  unsigned char image[32];
  int i;
  Pixmap Cursor_src, Cursor_msk;
  Screen *screen;

#ifdef BYTESWAP   
  if (from_lisp) for (i=0; i<32; i++) image[i] = reversedbits[bitmap[i^3]];
  else for ( i=0; i<32; i++) image[i] = reversedbits[bitmap[i]];
#else
  for ( i=0; i<32; i++) image[i] = reversedbits[bitmap[i]];
#endif BYTESWAP

  XLOCK;
  screen = ScreenOfDisplay( dsp->display_id, DefaultScreen( dsp->display_id ));
  Cursor_src = XCreatePixmapFromBitmapData(dsp->display_id,
					   dsp->DisplayWindow,
					   image, 16, 16,
					   BlackPixelOfScreen( screen ),
					   WhitePixelOfScreen( screen ),
					   1); /* Has to have a depth of 1! */
  Cursor_msk = XCreatePixmapFromBitmapData(dsp->display_id,
					   dsp->DisplayWindow,
					   image, 16, 16,
					   BlackPixelOfScreen( screen ),
					   WhitePixelOfScreen( screen ),
					   1); /* Has to have a depth of 1! */
  *return_cursor = XCreatePixmapCursor( dsp->display_id
				       , Cursor_src, Cursor_msk
				       , &cursor_fore_xcsd, &cursor_back_xcsd
				       , hotspot_x, hotspot_y );

  /* Should free these now (doc says server may not copy them) */
  XFreePixmap(dsp->display_id, Cursor_src);
  XFreePixmap(dsp->display_id, Cursor_msk);

  XFlush( dsp->display_id );
  XUNLOCK;

}				/* end set_Xcursor */
