/* @(#) dspsubrs.c Version 2.14 (4/21/92). copyright Venue & Fuji Xerox  */
static char *id = "@(#) dspsubrs.c	2.14 4/21/92";
/*** ADOPTED NEW VERSION ***/



/************************************************************************/
/*									*/
/*	Copyright 1989, 1990 Venue, Fuji Xerox Co., Ltd, Xerox Corp.	*/
/*									*/
/*	This file is work-product resulting from the Xerox/Venue	*/
/*	Agreement dated 18-August-1989 for support of Medley.		*/
/*									*/
/************************************************************************/

#include <stdio.h>
#ifndef NOPIXRECT
#include <sunwindow/window_hs.h>
#include <pixrect/memvar.h>
#endif /* NOPIXRECT */


#include "lispemul.h"
#include "lsptypes.h"
#include "lispmap.h"
#include "display.h"
#include "arith.h"
extern int DebugDSP;
extern int displaywidth, displayheight;

#ifdef XWINDOW
extern int Mouse_Included;
#endif /* XWINDOW */

extern int ScreenLocked;
extern DLword *EmCursorX68K,*EmCursorY68K;
extern int LastCursorX,LastCursorY;


/****************************************************
 *
 *	DSP_dspbout() entry of SUBRCALL 9 1
 *			called from (DSPBOUT X)
 *
 ****************************************************/

DSP_dspbout( args )
LispPTR	*args;		/* args[0] :	charcode	*/
  {
    putc( (args[0] & 0xFFFF) & 0x7f, BCPLDISPLAY );
  }



/****************************************************
 *
 *	DSP_showdisplay() entry of SUBRCALL 19 2
 *			called from (SHOWDISPLAY BASE RASTERWIDTH)
 *
 ****************************************************/

extern int DisplayInitialized ;

DSP_showdisplay( args )
LispPTR	*args;
{
    DisplayInitialized = 1;
}



/****************************************************
 *
 *	DSP_VideoColor() entry of SUBRCALL 66 1
 *			called from (VIDEOCLOR BLACKFLG)
 *
 ****************************************************/

DSP_VideoColor( args )
LispPTR	*args;		/* args[0] :	black flag	*/
{
    int invert;
#ifdef SUNDISPLAY
    return NIL;
#endif /* SUNDISPLAY */


#ifdef XWINDOW
    invert = args[0] & 0xFFFF;
    lisp_Xvideocolor( invert );
    if( invert )
		return ATOM_T;
    else
		return NIL;
#endif /* XWINDOW */
  return NIL;

}



extern struct cursor CurrentCursor;
extern int LispWindowFd;

extern int errno;


/****************************************************
 *
 *	DSP_Cursor() entry of SUBRCALL 64 2
 *			called from \HARDCURSORUP etc.
 *
 ****************************************************/
DSP_Cursor( args , argnum)
LispPTR *args;	int argnum;
			/* args[0] :	hot sopt X
			 * args[1] :	hot spot Y
			 */
{
    static int Init=T;

#ifdef XWINDOW
    Set_XCursor( (int)(args[0]&0xFFFF), (int)(args[1]&0xFFFF) );

#else
#ifndef DOS
    if (argnum == 2) {
	CurrentCursor.cur_xhot=args[0] & 0xffff;
	CurrentCursor.cur_yhot=args[1] & 0xffff;
	};
#endif /* DOS */
#ifdef OLD_CURSOR
    win_setcursor( LispWindowFd, &CurrentCursor );
#else
#ifndef INIT
    ScreenLocked =T;
    if(!Init){
	  taking_mouse_down();
	  taking_mouse_up(*EmCursorX68K,*EmCursorY68K);
	}
    else{
	  Init=NIL;
	  cursor_hidden_bitmap(0,0);
	  taking_mouse_up(0,0);
	  *EmCursorX68K=LastCursorX=0;
	  *EmCursorY68K=LastCursorY=0;
	}
    ScreenLocked=NIL;
#else
/* Init specific lde only */
    ScreenLocked =T;
    if(!Init){
	  taking_mouse_down();
	  taking_mouse_up(0,0);
	}
    else{
	  Init=NIL;
	  cursor_hidden_bitmap(0,0);
	  taking_mouse_up(0,0);
	}
/****
      taking_mouse_down();
      taking_mouse_up(0,0);
***/
    ScreenLocked=NIL;
#endif /* INIT */

#endif /* OLD_CURSOR */
#endif /* XWINDOWS vs SUN/DOS */


}



/****************************************************
 *
 *	DSP_SetMousePos() entry of SUBRCALL 65 2
 *			called from macro \SETMOUSEXY etc.
 *
 ****************************************************/

DSP_SetMousePos( args )
register LispPTR *args;	/* args[0] :	X pos
			 * args[1] :	Y pos
			 */
{
#ifdef SUNDISPLAY
#ifdef OLD_CURSOR
    register int x ,y;
    x=GetSmalldata(args[0]);
    y=GetSmalldata(args[1]); /* debug */
    win_setmouseposition(LispWindowFd, GetSmalldata(args[0]),
			                   GetSmalldata(args[1]));
#else
    extern int ScreenLocked;
    extern DLword *EmCursorX68K,*EmCursorY68K,*EmMouseX68K,*EmMouseY68K;
    register int x ,y;
    ScreenLocked=T;
    x=GetSmalldata(args[0]);
    y=GetSmalldata(args[1]);
    /* for Suntool's invisible cursor */
    win_setmouseposition(LispWindowFd, x,y);
    /* for REAL cursor image */
    taking_mouse_down();
    taking_mouse_up(x,y);

#ifndef INIT
    *EmMouseX68K=x;
    *EmMouseY68K=y;
#endif
    ScreenLocked=NIL;
#endif
#endif /* SUNDISPLAY */


#ifdef XWINDOW
    if( Mouse_Included )
		set_Xmouseposition( (int)(GetSmalldata(args[0]))
				  , (int)(GetSmalldata(args[1])) );
#endif /* XWINDOW */

#ifdef DOS
    set_DOSmouseposition((int)(GetSmalldata(args[0])),
			 (int)(GetSmalldata(args[1])) );
#endif DOS
}

/****************************************************
 *
 *	DSP_ScreenWidth() entry of SUBRCALL 67 0
 *			called from  \Katana.DisplayWidth.
 *
 ****************************************************/
DSP_ScreenWidth( args )
LispPTR *args;
{
	return( S_POSITIVE | (0xFFFF & displaywidth) );
}

/****************************************************
 *
 *	DSP_ScreenHight() entry of SUBRCALL 68 0
 *			called from  \Katana.DisplayHeight.
 *
 ****************************************************/
DSP_ScreenHight( args )
LispPTR *args;
{
	return( S_POSITIVE | (0xFFFF & displayheight) );
}

/****************************************************
 *
 *	flip_cursor()
 *
 ****************************************************/

extern DLword *EmCursorBitMap68K;
extern int for_makeinit;

flip_cursor()
{
register DLword *word;
register int cnt;
	word = EmCursorBitMap68K;

#ifdef INIT

	/* since this is called frequently, and you don't want to have
	   to build a different LDE to run the 2 parts of a Loadup, there is 
	   an ifdef AND a test.  This way we don't generate
	   extra code for anybody elses building an LDE
	   except those who want to try building loadups.  */

	if (!for_makeinit){
		for (cnt = CURSORHEIGHT;(cnt--);)
		   {
			GETWORD(word++) ^= 0xFFFF;
	 	   };
	};

#else

	for (cnt = CURSORHEIGHT;(cnt--);)
	  {
	    GETWORD(word++) ^= 0xFFFF;
	  };

#endif


#ifdef XWINDOW
	Set_XCursor( 0, 0 );

#else
	/* non-X version.  Suited for suns and dos */
#ifdef OLD_CURSOR

	win_setcursor( LispWindowFd, &CurrentCursor );
#else
	ScreenLocked=T;
	taking_mouse_down();
#ifndef INIT
	taking_mouse_up(*EmCursorX68K,*EmCursorY68K);
#else
	if(!for_makeinit)
	  taking_mouse_up(*EmCursorX68K,*EmCursorY68K);
	else
	  taking_mouse_up(0,0);
#endif /* INIT */

	ScreenLocked=NIL;
#endif
#endif /* XWINDOW vs sun/dos */



}
