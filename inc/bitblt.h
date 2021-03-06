/* @(#) bitblt.h Version 2.8 (4/21/92). copyright Venue & Fuji Xerox  */
/*
 *	Copyright (C) 1988 by Fuji Xerox Co., Ltd. All rights reserved.
 *
 *	File :  bitblt.h
 *
 *	Author :  Osamu Nakamura
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



#define	REPLACE	0
#define	PAINT	2
#define	ERASE	1
#define	INVERT	3
#define	ERROR	PIX_SRC

#define PixOperation( SRCTYPE, OPERATION )	\
	( SRCTYPE == ERASE ? \
		(OPERATION == REPLACE ? PIX_NOT(PIX_SRC) : \
		(OPERATION == PAINT   ? PIX_NOT(PIX_SRC) | PIX_DST : \
		(OPERATION == ERASE   ? PIX_NOT(PIX_SRC) & PIX_DST : \
		(OPERATION == INVERT  ? PIX_NOT(PIX_SRC) ^ PIX_DST : ERROR)))) : \
	/*  SRCTYPE == INPUT */ \
		(OPERATION == REPLACE ? PIX_SRC : \
		(OPERATION == PAINT   ? PIX_SRC | PIX_DST : \
		(OPERATION == ERASE   ? PIX_SRC & PIX_DST : \
		(OPERATION == INVERT  ? PIX_SRC ^ PIX_DST : ERROR)))))
/*----------------------------------------------------------------------*/

extern DLword	*EmMouseX68K, *EmMouseY68K;
extern int DisplayRasterWidth;
#define	XDELTA	50
#define	YDELTA	50
#define	MOUSEXL	((int)*EmMouseX68K - XDELTA)
#define	MOUSEXR	((int)*EmMouseX68K + XDELTA)
#define	MOUSEYL	((int)*EmMouseY68K - YDELTA)
#define	MOUSEYH	((int)*EmMouseY68K + YDELTA)

/*----------------------------------------------------------------------*/
#if (OLD_CURSOR && !XWINDOW)
extern struct winlock DisplayLockArea;
#define	HideCursor	\
	ioctl( LispWindowFd, WINLOCKSCREEN, &DisplayLockArea)

#define ShowCursor \
	ioctl( LispWindowFd, WINUNLOCKSCREEN, &DisplayLockArea)
#elif XWINDOW
#define HideCursor	{}
#define ShowCursor	{}
#else
extern DLword *EmCursorX68K,*EmCursorY68K;
#define	HideCursor	{ taking_mouse_down();}
#define ShowCursor	{ taking_mouse_up(*EmCursorX68K,*EmCursorY68K);}
#endif /* OLD_CURSOR or XWINDOW */

#ifdef XWINDOW
#define refresh_CG6	clipping_Xbitblt(dsp, dsp->UpperLeftX, \
					 dsp->UpperLeftY, dsp->VissibleWidth, dsp->VissibleHeight);
#define clear_CG6	{}
#else

#define refresh_CG6 \
	HideCursor; \
	pr_rop(ColorDisplayPixrect, 0, 0, displaywidth, displayheight, \
			PIX_SRC,DisplayRegionPixrect, 0, 0); \
	ShowCursor;

#define clear_CG6 \
	HideCursor; \
	pr_rop(ColorDisplayPixrect, 0, 0, displaywidth, displayheight, \
			PIX_CLR, ColorDisplayPixrect, 0, 0); \
	ShowCursor;
#endif /* XWINDOW */
