/* @(#) keyevent.c Version 2.39 (4/21/92). copyright Venue & Fuji Xerox  */
static char *id = "@(#) keyevent.c	2.39 4/21/92		(venue & Fuji Xerox)";



/************************************************************************/
/*									*/
/*	(C) Copyright 1989-92 Venue. All Rights Reserved.		*/
/*	Manufactured in the United States of America.			*/
/*									*/
/*	The contents of this file are proprietary information 		*/
/*	belonging to Venue, and are provided to you under license.	*/
/*	They may not be further distributed or disclosed to third	*/
/*	parties without the specific permission of Venue.		*/
/*									*/
/************************************************************************/

/*
 *	This file contains the routines that interface Lisp to the
 *	Sun keyboard and mouse.
 *
 */

#include	<stdio.h>
#include	<signal.h>
#include	<sys/types.h>
#ifndef DOS
#include	<sys/file.h>
#include	<sys/ioctl.h>
#include <sys/time.h>
#else
#include <time.h>
#endif /* DOS */
#ifdef XWINDOW
#define TRUE 1
#define FALSE 0
#include "dspif.h"
extern DspInterface currentdsp, colordsp;

#elif DOS

#include <i32.h>                             /* Defines "#pragma interrupt"  */
#include <dos.h>                             /* Defines REGS & other structs */
#include <stk.h>                             /* _XSTACK struct definition    */
#include "dosmse.h"                        /* Mouse defines                */
#pragma interrupt(Mouse_hndlr)

void  Mouse_hndlr(void);           /* Fields mouse events from driver        */
                                   /*  (during servicing of mouse interrupt) */

#else
#include	<sunwindow/window_hs.h>
#include	<sunwindow/win_ioctl.h>
#include	<suntool/window.h>
#include	<ctype.h>
#include	<sundev/kbio.h>
#endif /* DOS XWINDOW */


#ifdef ISC
#include <sys/bsdtypes.h>
#include <signal.h>
#define SIGIO SIGPOLL
#endif /* ISC */


#include "lispemul.h"
#include "lspglob.h"
#include "adr68k.h"
#include "address.h"
#include "stack.h"
#include "keyboard.h"
#include "display.h"
#include "lsptypes.h"


#include "iopage.h"
#include "ifpage.h"

#include "bb.h"
#include "bitblt.h"
#include "pilotbbt.h"

#include "dbprint.h"

/* for contextsw */
#define AS_OPCODE	1
#define AS_CPROG	0

/*  EmMouseX68K are already swapped, no need for GETWORD */
#define MouseMove(x,y) {*((DLword *)EmMouseX68K) = x;	\
			*((DLword *)EmMouseY68K) = y;}

#ifndef BYTESWAP
#define PUTBASEBIT68K(base68k, offset, bitvalue ) { \
	if( bitvalue) \
		*((DLword*)(base68k) + (((u_short)(offset))>>4 )) \
		|=    1 << (15 - ((u_short)(offset))%BITSPER_DLWORD); \
	    else	 \
		*((DLword*)(base68k) + (((u_short)(offset))>>4 ))  \
		&= ~( 1 << (15 - ((u_short)(offset)) %BITSPER_DLWORD)); \
	}
#else

/* convert to real 68 k address, then do arithmetic, and convert
   back to i386 address pointer */

#define PUTBASEBIT68K(base68k, offset, bitvalue ) { \
    int real68kbase;		\
    real68kbase = 2 ^ ((int)(base68k));		\
	if( bitvalue) \
		GETWORD((DLword*)(real68kbase) + (((u_short)(offset))>>4 )) \
		|=    1 << (15 - ((u_short)(offset))%BITSPER_DLWORD); \
	    else	 \
		GETWORD((DLword*)(real68kbase) + (((u_short)(offset))>>4 ))  \
		&= ~( 1 << (15 - ((u_short)(offset)) %BITSPER_DLWORD)); \
	}
#endif

extern DLword  *EmMouseX68K, *EmMouseY68K, *EmKbdAd068K, *EmRealUtilin68K,*EmUtilin68K;
extern DLword  *EmKbdAd168K,*EmKbdAd268K,*EmKbdAd368K,*EmKbdAd468K,*EmKbdAd568K;
extern u_char *SUNLispKeyMap;
extern u_int   LispReadFds, LispWindowFd;
extern int     RS232CReadFds, RS232C_remain_data;
#ifdef NOETHER
extern u_int	LogFileFd;
#else
extern int	ether_fd;
extern u_int	LogFileFd, EtherReadFds;
#endif /* NOETHER */

extern DLword *DisplayRegion68k;

u_int LispIOFds = 0;
#ifndef DOS
static	struct timeval	SelectTimeout = {
	0, 0	
};
#endif /* DOS */


#ifdef XWINDOW
static int Xcount=1;
extern int Event_Req;
#endif /* XWINDOW */



extern MISCSTATS *MiscStats;
 LispPTR *LASTUSERACTION68k;
 LispPTR *CLastUserActionCell68k;
 LispPTR *CURSORDESTHEIGHT68k;
 LispPTR *CURSORDESTWIDTH68k;

 LispPTR *CURSORHOTSPOTX68k;
 LispPTR *CURSORHOTSPOTY68k;
 LispPTR *SOFTCURSORUPP68k;
 LispPTR *SOFTCURSORWIDTH68k;
 LispPTR *SOFTCURSORHEIGHT68k;
 LispPTR *CURRENTCURSOR68k;

extern DLword *EmCursorX68K;
extern DLword *EmCursorY68K;

#ifndef BYTESWAP
typedef struct
  {
    unsigned nil : 8;
    unsigned type: 8;
    unsigned num : 16;
  } SNum;
#else
typedef struct
  {
    unsigned num : 16;
    unsigned type: 8;
    unsigned nil : 8;
  } SNum;

#endif /* BYTESWAP */

#define  IDiff(x68k,y68k)	(((SNum*)(x68k))->num - ((SNum*)(y68k))->num)

/*  EmXXXX68K are already swapped, no need for GETWORD */

#ifdef OLD_CURSOR
#define TrackCursor(cx,cy)	{*CLastUserActionCell68k = MiscStats->secondstmp;\
	*EmCursorX68K = cx;	\
	*EmCursorY68K = cy;}
#else
#define TrackCursor(cx,cy)	{*CLastUserActionCell68k = MiscStats->secondstmp;\
	taking_mouse_down();\
	taking_mouse_up(cx,cy); \
	*EmCursorX68K= cx;\
	*EmCursorY68K= cy;}
#endif /* OLD_CURSOR */


/* function that checks to see if more input is waiting on a file;
	if some is, it bumps FileIOFlag so it'll get seen next time around */
/* commented out is some code that would also clobber 
	Irq_Stk_Check & Irq_Stk_End to force
	a new interrupt as rapidly as possible; it causes odd behavior...
	needs some study and thought */

/* this is currently called EVERY time the timer expires. It checks for
   keyboard input */


LispPTR *MOUSECHORDTICKS68k;

/**NEW GLOBAL***-> will be moved***/
LispPTR *KEYBOARDEVENTQUEUE68k;
LispPTR *KEYBUFFERING68k;
int	KBDEventFlg=NIL;
DLword *CTopKeyevent;

LispPTR DOBUFFEREDTRANSITION_index;
LispPTR INTERRUPTFRAME_index;
LispPTR *TIMER_INTERRUPT_PENDING68k;
LispPTR *PENDINGINTERRUPT68k;
LispPTR ATOM_STARTED;
LispPTR *PERIODIC_INTERRUPT68k;
LispPTR *PERIODIC_INTERRUPT_FREQUENCY68k;
LispPTR PERIODIC_INTERRUPTFRAME_index;
LispPTR DORECLAIM_index;

LispPTR *IOINTERRUPTFLAGS_word;

int URaid_req=NIL;
int ScreenLocked=NIL;

DLword MonoCursor_savebitmap[CURSORHEIGHT]; /* for mono cursor save-image */
#define COLOR_DEPTH 8
#define COLORPIXELS_IN_LONGWORD 4
#define COLORPIXELS_IN_DLWORD 2
DLword ColorCursor_savebitmap[CURSORWIDTH/COLORPIXELS_IN_DLWORD * CURSORHEIGHT];

/************************************************************************/
/*									*/
/*			G E T S I G N A L D A T A			*/
/*									*/
/*	Handler for the SIGIO interrupt, which happens			*/
/*		1. When a key transition happens			*/
/*		2. On mouse moves					*/
/*		3. When TCP input becomes available.			*/
/*		4. When a NIT ethernet packet becomes available.	*/
/*		5. When a console/log/stderr msg needs to be printed.	*/
/*									*/
/*									*/
/*									*/
/*									*/
/*									*/
/*									*/
/*									*/
/*									*/
/*	Statics:  LispReadFds	A 32-bit vector with a 1 for each	*/
/*				FD that can get SIGIO interrupts.	*/
/*									*/
/*		  LispWindowFd	The keyboard/window FD, for keyboard	*/
/*				and mouse events.			*/
/*									*/
/*		  LispIOFds	A bit vector of TCP FDs, or other	*/
/*				FDs Lisp is doing async I/O on.		*/
/*				Activity on any of these will signal	*/
/*				the Lisp IOInterrupt interrupt.		*/
/*									*/
/*		  ether_fd	The raw ethernet FD, for 10MB I/O	*/
/*									*/
/*		  EtherReadFds	A bit vector with the raw enet's	*/
/*				bit turned on.  To speed up processing.	*/
/*									*/
/*		  LogFileFd	A bit vector with the log-file's	*/
/*				bit on, for capturing console msgs.	*/
/*									*/
/*									*/
/*									*/
/************************************************************************/

getsignaldata(sig, code, scp)
  int sig, code;
  struct sigcontext *scp;
  {
#ifndef DOS
#ifndef XWINDOW
    struct inputevent event;
#endif /* XWINDOW */

    static int  rfds, wfds, efds;

/* #ifndef KBINT */
    rfds = LispReadFds;
    efds = LispReadFds;
   /* label and ifs not needed if only keyboard on SIGIO */
getmore: 
    if(select(32, &rfds, NULL, &efds, &SelectTimeout) >= 0 )
      {
	DBPRINT(("SIGIO: fd mask(r/e) = 0x%x/0x%x.\n", rfds, efds));

#ifdef SUNDISPLAY
	if( rfds & (1 << LispWindowFd) )
	  {
/* #endif */
	    while( input_readevent(LispWindowFd, &event) >= 0 )
	      {
	        /*if(!kb_event( &event )) {goto getmore;};*/
	        if((KBDEventFlg+=kb_event( &event ))>0)
	          {
		    /* immidiately request for IRQ check */
		    Irq_Stk_End=Irq_Stk_Check=0;
		  }
	      }
/* #ifndef KBINT */
	  }
#endif /* SUNDISPLAY */

#ifdef XWINDOW
    if (rfds & (1 << ConnectionNumber(currentdsp->display_id)))
      {
	getXsignaldata(currentdsp);
      }
    else if (colordsp->display_id)
      if (rfds & (1 << ConnectionNumber(colordsp->display_id)))
	{
	  getXsignaldata(colordsp);
	}

#endif /* XWINDOW */

#ifdef NOETHER
#else
	if (rfds & EtherReadFds)
	  { /* Raw ethernet (NIT) I/O happened, so handle it. */
	    DBPRINT(("Handling enet interrupt.\n\n"));
	    check_ether();
	  }
#endif /* NOETHER */


#ifdef RS232
	if (((rfds & RS232CReadFds) == RS232CReadFds) ||
	    (RS232C_remain_data && rs232c_lisp_is_ready())) rs232c_read();
#endif /* RS232 */


#ifdef LOGINT
	if (rfds & LogFileFd)
	  { /* There's info in the log file.  Tell Lisp to print it. */
	    flush_pty();	/* move the msg(s) to the log file */
	    
	   ((INTSTAT*)Addr68k_from_LADDR(*INTERRUPTSTATE_word))
		->LogFileIO = 1;

	    *PENDINGINTERRUPT68k = ATOM_T;
	    Irq_Stk_End=Irq_Stk_Check=0;
	  }
#endif
	if (rfds & LispIOFds)
	  { /* There's activity on a Lisp-opened FD.  Tell Lisp. */
	    u_int *flags;
	    flags = (u_int *)Addr68k_from_LADDR(*IOINTERRUPTFLAGS_word);
	    *flags = rfds & LispIOFds;
	    
	   ((INTSTAT*)Addr68k_from_LADDR(*INTERRUPTSTATE_word))
		->IOInterrupt = 1;

	    *PENDINGINTERRUPT68k = ATOM_T;
	    Irq_Stk_End=Irq_Stk_Check=0;
	  }

      }
/* #endif */
#endif /* DOS */
  } /* end getsignaldata */




/************************************************************************/
/*									*/
/*			    k b _ e v e n t				*/
/*									*/
/*	Given an event from the kbd code, return 1 if a key transition	*/
/*	occurred, 0 if one didn't occur.				*/
/*									*/
/************************************************************************/

#ifdef DOS
    /* for DOS, need to define these pseudo-keynumbers */
#define MS_LEFT   0xCC10
#define MS_MIDDLE 0xCC20
#define MS_RIGHT  0xCC30
#endif /* DOS */


#ifndef XWINDOW
extern int for_makeinit;

int kb_event( event )
#ifdef DOS
register unsigned int event;	/* actually the key scan code */
#else
register struct inputevent *event;
#endif
  {
    register u_int  upflg;
    int kn;
    DLword w,r;
    KBEVENT *kbevent;

#ifdef INIT

/* generate some code to check if we are running as an INIT.  Don't
   needlessly generate this code, and don't return if we aren't
   running with the -INIT flag turned on.  --was 2/7/89 */

    if (for_makeinit)
       {
	return(0);
       };

#endif

#ifdef DOS
    if ((event & 0xCC00) == 0xCC00) { upflg = event & 1; event &= 0xfffffe; }
	else { upflg = (event > 0x80);  event &= 0x7f; }
#else
    upflg = event_is_up( event );
#endif /* DOS */

#ifdef SHOWKEYSTROKES
    printf("Key # %d, upflg %d.\n", (unsigned short)event->ie_code, upflg); 
#endif

#ifdef DOS
	switch (event)
#else
	switch( ((unsigned short)event->ie_code) )
#endif /* DOS */
      {
#ifndef DOS
	case LOC_MOVE:
#ifndef OLD_CURSOR
		if(!ScreenLocked)
#endif
		  { ScreenLocked=T;
		    MouseMove( event->ie_locx, event->ie_locy );
		    TrackCursor(event->ie_locx,event->ie_locy );
		    ScreenLocked=NIL;
		  }
		return(0);
#endif /* DOS */

	case MS_LEFT:  /*mouse_button( MOUSE_LEFT, upflg );*/
		PUTBASEBIT68K( EmRealUtilin68K, MOUSE_LEFT, upflg );
		break;
			
	case MS_MIDDLE:  /*mouse_button( MOUSE_MIDDLE, upflg );*/
		PUTBASEBIT68K( EmRealUtilin68K, MOUSE_MIDDLE, upflg );
		break;
			
	case MS_RIGHT:  /*mouse_button( MOUSE_RIGHT, upflg );*/
		PUTBASEBIT68K( EmRealUtilin68K, MOUSE_RIGHT, upflg );
		break;

	default:  /* keystroke */
#ifdef DOS
		if((kn=SUNLispKeyMap[((unsigned short)event)]) > -1)
#else
		if((kn=SUNLispKeyMap[((unsigned short)event->ie_code)]) > -1)
#endif /* DOS */
				kb_trans( kn, upflg );
		else
			printf("kb_event: unknown key number=%d\n",
#ifdef DOS
				event
#else
				event->ie_code
#endif /* DOS */
			       );
		break;
      };
      {
    do_ring:
	/* Emxxx do not use GETWORD */
	if(((*EmKbdAd268K) & 2113) == 0)
	  {/*Ctrl-shift-NEXT*/
	    error("******  EMERGENCY Interrupt ******");
	    (*EmKbdAd268K)=KB_ALLUP; /*reset*/
	    ((RING*)CTopKeyevent)->read=0;  /* reset queue */
	    ((RING*)CTopKeyevent)->write=MINKEYEVENT; 
	    /*return(0);*/
	  } 
	else if(((*EmKbdAd268K) & 2114) == 0)
	  { /* Ctrl-Shift-DEL */
	    (*EmKbdAd268K)=KB_ALLUP; /*reset*/
	    URaid_req=T;
	    ((RING*)CTopKeyevent)->read=0;  /* reset queue */
	    ((RING*)CTopKeyevent)->write=MINKEYEVENT; 

	    /*return(0);*/
	  }

#ifdef OS4_TYPE4BUG
	else if(((*EmKbdAd268K) & 2120) == 0)
	  { /* Ctrl-Shift-Return */
	    error("******  EMERGENCY Interrupt ******");
	    (*EmKbdAd268K)=KB_ALLUP; /*reset*/
	    ((RING*)CTopKeyevent)->read=0;  /* reset queue */
	    ((RING*)CTopKeyevent)->write=MINKEYEVENT; 
	    /*return(0);*/
	  }
#endif

	r=RING_READ(CTopKeyevent);
	w=RING_WRITE(CTopKeyevent);

	if(r==w) goto KBnext; /* event queue FULL */

	kbevent=(KBEVENT*)(CTopKeyevent+ w);

/*	RCLK(kbevent->time); */

	kbevent->W0= (*EmKbdAd068K);	/* Emxxxx do not use GETWORD */
	kbevent->W1= (*EmKbdAd168K);
	kbevent->W2= (*EmKbdAd268K);
	kbevent->W3= (*EmKbdAd368K);
	kbevent->W4= (*EmKbdAd468K);
	kbevent->W5= (*EmKbdAd568K);
	kbevent->WU= (*EmRealUtilin68K);

	if(r==0) /* Queue was empty */ ((RING*)CTopKeyevent)->read=w;
	if(w >= MAXKEYEVENT) ((RING*)CTopKeyevent)->write = MINKEYEVENT;
	else ((RING*)CTopKeyevent)->write = w + KEYEVENTSIZE;

    KBnext:
	if(*KEYBUFFERING68k ==NIL) *KEYBUFFERING68k=ATOM_T;

#ifdef DOS
    /* For DOS, we're not using getsignaldata, so this has to happen here. */
    KBDEventFlg++;
    Irq_Stk_End=0;
    Irq_Stk_Check=0;
#endif /* DOS */

      } /* if *EmRealUtilin68K end */
    return(1);
  } 
#endif /* XWINDOW */



/************************************************************************/
/*									*/
/*			   k b _ t r a n s				*/
/*									*/
/*	Return the transition code??					*/
/*									*/
/************************************************************************/

kb_trans( keycode, upflg )
  u_short keycode;
  u_short upflg;
{
  extern IFPAGE *InterfacePage;
  if( keycode < 64 )		/* DLKBDAD0 ~ 3	*/
    { PUTBASEBIT68K( EmKbdAd068K, keycode, upflg );}
  else if( keycode >= 80 )	/* DLKBDAD4, 5	*/
    { PUTBASEBIT68K( EmKbdAd068K, keycode - 16, upflg );}
  else if( keycode >= 64 && keycode < 80 ) /* DLUTILIN	*/
    {
      PUTBASEBIT68K( EmRealUtilin68K, (keycode & 15), upflg );
      PUTBASEBIT68K( EmUtilin68K, (keycode & 15), upflg );
    }
}




/**********************************************************/
/* 
	MOUSE tracking
*/
/**********************************************************/


typedef struct
  {
    LispPTR CUIMAGE;
    LispPTR CUMASK;
    LispPTR CUHOTSPOTX;
    LispPTR CUHOTSPOTY;
    LispPTR CUDATA;
  } CURSOR ;

#define DLWORD_PERLINE	(displaywidth/16)
#define HARD_CURSORWIDTH 16
#define HARD_CURSORHEIGHT 16
#define COLOR_BITSPER_PIXEL 8
/* For MonoOrColor */
#define MONO_SCREEN 0
#define COLOR_SCREEN 1
#define CursorClippingX(posx,width) {				\
	if( displaywidth < (posx + HARD_CURSORWIDTH)) {		\
 	 LastCursorClippingX=width=displaywidth-posx;}		\
	else { LastCursorClippingX=width=HARD_CURSORWIDTH;}}

#define CursorClippingY(posy,height){				\
	if(displayheight < (posy + HARD_CURSORHEIGHT)) {	\
	 LastCursorClippingY = height = displayheight - posy;}	\
	else { LastCursorClippingY = height = HARD_CURSORHEIGHT;}} 

extern int displaywidth, displayheight;
extern int DisplayInitialized ;
extern int MonoOrColor; /* MONO_SCREEN or COLOR_SCREEN */
int LastCursorClippingX=HARD_CURSORWIDTH;
int LastCursorClippingY = HARD_CURSORHEIGHT;
int LastCursorX=0;
int LastCursorY=0;


#ifndef COLOR
/* FOR MONO ONLY */
taking_mouse_down()
  {
    register DLword	*srcbase, *dstbase;
    static int sx, dx, w, h, srcbpl, dstbpl, backwardflg=0;
    static int src_comp=0, op=0, gray=0, num_gray=0, curr_gray_line=0;

    if(!DisplayInitialized) return;

    /* restore saved image */
    srcbase=MonoCursor_savebitmap;
    dstbase=DisplayRegion68k +( (LastCursorY) * DLWORD_PERLINE); /* old y */
    sx=0;
    dx = LastCursorX; /* old x */
    w = LastCursorClippingX; /* Old clipping */
    h= LastCursorClippingY;
    srcbpl =HARD_CURSORWIDTH;
    dstbpl =displaywidth;
    op=0;
    new_bitblt_code;
#ifdef DISPLAYBUFFER
    flush_display_region(dx, (LastCursorY), w, h);
#endif
  }
#else

			/* For COLOR & MONO */
extern DLword *ColorDisplayRegion68k;
  /* It assumes that MONO screen size and COLOR screen size are identical */
taking_mouse_down()
  {
    register DLword	*srcbase, *dstbase;
    static int sx, dx, w, h, srcbpl, dstbpl, backwardflg=0;
    static int src_comp=0, op=0, gray=0, num_gray=0, curr_gray_line=0;

    if(!DisplayInitialized) return;
    /* restore saved image */
    sx=0;

    if(MonoOrColor == MONO_SCREEN)
      {
	dx = LastCursorX; /* old x */
	srcbase=MonoCursor_savebitmap;
	dstbase=DisplayRegion68k +( (LastCursorY) * DLWORD_PERLINE); /* old y */
	w = LastCursorClippingX; /* Old clipping */
	h= LastCursorClippingY;
	srcbpl =HARD_CURSORWIDTH;
	dstbpl =displaywidth;
      }
    else
      {
	dx = LastCursorX * COLOR_BITSPER_PIXEL; /* old x */
	srcbase=ColorCursor_savebitmap;
	dstbase= ColorDisplayRegion68k +
	   ((LastCursorY) * DLWORD_PERLINE * COLOR_BITSPER_PIXEL); /* old y */
	w = LastCursorClippingX * COLOR_BITSPER_PIXEL; /* Old clipping */
	h= LastCursorClippingY;
	srcbpl =HARD_CURSORWIDTH * COLOR_BITSPER_PIXEL;
	dstbpl =displaywidth * COLOR_BITSPER_PIXEL ;
      }
    op=0;
    new_bitblt_code;
#ifdef DISPLAYBUFFER
    if( MonoOrColor == MONO_SCREEN )
    	flush_display_region(dx, LastCursorY, w, h);
#endif
}
#endif /* COLOR */


/* I'll make it MACRO */
taking_mouse_up(newx,newy)
  int newx,newy;
  {
    if(!DisplayInitialized) return;
    /* save hidden bitmap */
    cursor_hidden_bitmap(newx,newy);
    /* Copy Cursor Image */
#ifndef INIT
    copy_cursor(newx,newy);
#endif 
    LastCursorX=newx;
    LastCursorY=newy;
  }

/* LastCursorClippingX must be set brfore calling 
 To avoid duplicate caluculation */
#ifndef COLOR
/* FOR MONO ONLY */
copy_cursor(newx,newy)
  int newx,newy;
  {
    register DLword	*srcbase, *dstbase;
    register int offsetx,offsety;
    static int	sx, dx, w, h, srcbpl, dstbpl, backwardflg=0;
    static int	src_comp=0, op=0, gray=0, num_gray=0, curr_gray_line=0;
    extern DLword *EmCursorBitMap68K;
    /* copy cursor image */
    srcbase = EmCursorBitMap68K;
    dstbase= DisplayRegion68k +(newy * DLWORD_PERLINE) ;
    sx=0;
    dx=newx;
    w=LastCursorClippingX;
    h=LastCursorClippingY;;
    srcbpl=HARD_CURSORWIDTH;
    dstbpl=displaywidth;
    op=2; /* OR-in */
    new_bitblt_code;
#ifdef DISPLAYBUFFER
    flush_display_region(dx, newy, w, h);
#endif
  }

/* store bitmap image inside rect. which specified by x,y */
cursor_hidden_bitmap(x,y)
  int x,y;
  {
    register DLword	*srcbase, *dstbase;
    static int	sx, dx, w, h, srcbpl, dstbpl, backwardflg=0;
    static int	src_comp=0, op=0, gray=0, num_gray=0, curr_gray_line=0;
    /* save image */
    srcbase=DisplayRegion68k +( y* DLWORD_PERLINE) ;
    dstbase=MonoCursor_savebitmap;
    sx=x;
    dx=0;
    CursorClippingX(x,w); /* w and LastCursorClippingX rest */
    CursorClippingY(y,h); /* h and LastCursorClippingY reset */
    srcbpl =displaywidth;
    dstbpl=HARD_CURSORWIDTH;
    op= 0; /* replace */
    new_bitblt_code;
  }

#else
/* For COLOR & MONO */
#define IMIN(x,y)  (((x) > (y)) ? (y) : (x))
copy_cursor(newx,newy)
  int newx,newy;
{
  register DLword	*srcbase, *dstbase;
  register int offsetx,offsety;
  static int	sx, dx, w, h, srcbpl, dstbpl, backwardflg=0;
  static int	src_comp=0, op=0, gray=0, num_gray=0, curr_gray_line=0;
  CURSOR *cursor68k;
  BITMAP *bitmap68k;
  extern DLword *EmCursorBitMap68K;
  /* copy cursor image */
  if(MonoOrColor == MONO_SCREEN)
    {
      srcbase = EmCursorBitMap68K;
      dstbase= DisplayRegion68k +(newy * DLWORD_PERLINE) ;
      sx=0;
      dx=newx;
      w=LastCursorClippingX;
      h=LastCursorClippingY;;
      srcbpl=HARD_CURSORWIDTH;
      dstbpl=displaywidth;
    }
  else
    {
      cursor68k = (CURSOR*)Addr68k_from_LADDR(*CURRENTCURSOR68k);
      bitmap68k=(BITMAP*)Addr68k_from_LADDR(cursor68k->CUIMAGE);
      srcbase =(DLword*) Addr68k_from_LADDR(bitmap68k->bmbase);
      dstbase=  ColorDisplayRegion68k +(newy * DLWORD_PERLINE * COLOR_BITSPER_PIXEL) ;
      sx=0;
      dx=newx * COLOR_BITSPER_PIXEL;
      w=IMIN(LastCursorClippingX,LOLOC(bitmap68k->bmwidth)) * COLOR_BITSPER_PIXEL;
      h=IMIN(LastCursorClippingY,LOLOC(bitmap68k->bmheight));
      /* srcbpl=HARD_CURSORWIDTH * COLOR_BITSPER_PIXEL;*/
      srcbpl = bitmap68k->bmwidth * COLOR_BITSPER_PIXEL;
      dstbpl=displaywidth * COLOR_BITSPER_PIXEL;
    }
  op=2;				/* OR-in */
  new_bitblt_code;
#ifdef DISPLAYBUFFER
  if( MonoOrColor == MONO_SCREEN )
    flush_display_region(dx, newy, w, h);
#endif
}

/* store bitmap image inside rect. which specified by x,y */
cursor_hidden_bitmap(x,y)
int x,y;
{
  register DLword	*srcbase, *dstbase;
  static int	sx, dx, w, h, srcbpl, dstbpl, backwardflg=0;
  static int	src_comp=0, op=0, gray=0, num_gray=0, curr_gray_line=0;
  /* save image */
  if(MonoOrColor == MONO_SCREEN) {
    srcbase=DisplayRegion68k +( y* DLWORD_PERLINE) ;
    dstbase=MonoCursor_savebitmap;
    sx=x;
    dx=0;
    CursorClippingX(x,w);	/* w and LastCursorClippingX rest */
    CursorClippingY(y,h);	/* h and LastCursorClippingY reset */
    srcbpl =displaywidth;
    dstbpl=HARD_CURSORWIDTH;
  }
  else {
    srcbase=ColorDisplayRegion68k +(y * DLWORD_PERLINE * COLOR_BITSPER_PIXEL);
    dstbase= ColorCursor_savebitmap;
    sx=x * COLOR_BITSPER_PIXEL ;
    dx=0;
    CursorClippingX(x,w);	/* w and LastCursorClippingX rest */
    CursorClippingY(y,h);	/* h and LastCursorClippingY reset */
    w = w * COLOR_BITSPER_PIXEL;
    srcbpl =displaywidth * COLOR_BITSPER_PIXEL ;
    dstbpl=HARD_CURSORWIDTH * COLOR_BITSPER_PIXEL;
  }
  op= 0;			/* replace */
  new_bitblt_code;
}
#endif /* COLOR */



/************************************************************************/
/*									*/
/*		D O S   M o u s e   H a n d l i n g			*/
/*									*/
/*	init_DOSmouse()  turns on the mouse driver & installs an	*/
/*			 interrupt handler to take mouse events.	*/
/*									*/
/*	disable_DOSmouse() Turns off mouse handling temporarily		*/
/*									*/
/*	enable_DOSmouse()  Turns mouse handling back on 		*/
/*									*/
/*	mouse_hndlr()	   The mouse-interrupt handler function		*/
/*									*/
/*	DOSmouseON	   Variable-- T if mouse active, NIL if not.	*/
/*									*/
/*	set_DOSmouseposition(x,y) Set mouse position to X, Y.		*/
/*									*/
/*									*/
/*									*/
/*									*/
/************************************************************************/
#ifdef DOS
int DOSmouseON = 0;


void init_DOSmouse()
   {
   union REGS regs;


   /***************************************************************************
   * Reset mouse driver, exit if no mouse driver present
   ***************************************************************************/
   regs.w.eax = 0;                    /* Function 0 = reset the mouse driver */
   int86(0x33, &regs, &regs);

   if (regs.w.eax != -1) {    /* Check if mouse driver installed (-1 if not) */
      printf("Mouse driver must be installed before running Medley.\n");
      exit(1);
      }


   regs.w.eax = 0x0001;                 /* Function 1 = turn on mouse cursor */
   int86(0x33, &regs, &regs);
   regs.w.eax = 0x0002;                 /* Function 2 = hide mouse cursor */
   int86(0x33, &regs, &regs);



   /***************************************************************************
   * Lock the "user-defined" handler and its data before installing it.
   * This prevents the handler from being "swapped out" at a critical time.
   ***************************************************************************/

   _dpmi_lockregion((void *)&Mouse_hndlr, 4096);

   /***************************************************************************
   * Install "user-defined" handler for events: left, right button pressed
   ***************************************************************************/
   regs.w.eax = 0x0C;        /* Function 0C = set user-defined mouse handler */
   regs.w.ecx = LB_PRESS | RB_PRESS | CB_PRESS
	      | LB_OFF   | RB_OFF   | CB_OFF    | MOUSE_MV;
   regs.w.edx = FP_OFF(Mouse_hndlr); /* Address of our mouse handler routine */
   int86(0x33, &regs, &regs);       /* Install our handler to process events */

   if (regs.w.eax == 'MERR') {       /* Check for error return "code" in eax */
      printf("Unable to install mouse handler - not enough low memory.\n");
      exit(1);                         /* Uninstall intercept & exit */
      }

    DOSmouseON = 1;	/* enable mouse handling */

  regs.x.ax = 0x7;		/* Set mouse horizontal range */
  regs.x.cx = 0x0;
  regs.x.dx = (short)( displaywidth - 1);
  int86(0x33, &regs, &regs);

  regs.x.ax = 0x8;		/* Set mouse vertical range */
  regs.x.cx = 0x0;
  regs.x.dx = (short)(displayheight - 1 );
  int86(0x33, &regs, &regs);

  regs.x.ax = 0xf;		/* Set mickey per pixel range */
  regs.x.cx = 0x8;
  regs.x.dx = 0x8;
  int86(0x33, &regs, &regs);

   }

void disable_DOSmouse()
  {
    DOSmouseON = 0;
  }

void enable_DOSmouse()
  {
    DOSmouseON = 1;
  }


/******************************************************************************
* Mouse_hndlr
*
* This "user-installed" handler is invoked by the mouse driver whenever an
* event occurs which matches one of the events specified when the handler
* was installed.
*
* Note: The handler must be declared using "#pragma interrupt()" in order
* to retrieve the mouse driver's registers from the _XSTACK frame.  Also
* note that the mouse handler should not make any DOS or BIOS interrupt calls,
* either directly, or indirectly through a C library call.
*
* As currently written, the mouse handler is blocked from storing the event
* information if the previous event has not yet been processed by the main
* program.  If new events are added, it may be necessary for the handler to
* queue the mouse events to ensure that none are missed.
*
* Input:
*    _XSTACK - Stack frame containing driver's registers:
*       eax - Mouse event flags, as follows:
*             Bit   Meaning if set
*             ===   ==============
*             0     Mouse movement
*             1     Left button pressed
*             2     Left button released
*             3     Right button pressed
*             4     Right button released
*             5     Center button pressed
*             6     Center button released
*       ebx - Button state, as follows:
*             Bit   Meaning if set
*             ===   ==============
*             0     Left button is down
*             1     Right button is down
*             2     Center button is down
*       ecx - Horizontal (X) mouse pointer coordinate.
*       edx - Vertical (Y) mouse pointer coordinate.
*       esi - Last raw vertical mickey count.
*       edi - Last raw horizontal mickey count.
*    Although all values are stored in 32-bit locations, only the lower
*    16-bits of information contain data from the mouse driver.
*
*  Output:
*    drvr_flags     -  Event flag(s) (from ax) representing mouse event(s).
*    mouse_event    -  TRUE to indicate a new mouse event has occurred.
******************************************************************************/
void  Mouse_hndlr(void)
  {
    _XSTACK  *stk_ptr;
    unsigned short mouse_flags;
    unsigned short locx, locy;

    /* First save the stack frame. */
    stk_ptr = (_XSTACK *)_get_stk_frame(); /* Get ptr to V86 _XSTACK frame */

    /* Stackframe saved, now we can test for initialization. */
    if (!DOSmouseON) return;

    mouse_flags = stk_ptr->eax;       /* Save event flags from mouse driver */

    if (mouse_flags & LB_PRESS) kb_event( MS_LEFT );
    if (mouse_flags & LB_OFF) kb_event( MS_LEFT | 1 );

    if (mouse_flags & CB_PRESS) kb_event( MS_MIDDLE| 0 );
    if (mouse_flags & CB_OFF) kb_event( MS_MIDDLE| 1 );

    if (mouse_flags & RB_PRESS) kb_event( MS_RIGHT| 0 );
    if (mouse_flags & RB_OFF) kb_event( MS_RIGHT| 1 );

    if (mouse_flags & MOUSE_MV)
      {
	/* The mouse has moved; track it. */
#ifndef OLD_CURSOR
	if(!ScreenLocked)
#endif
	  { ScreenLocked=T;
	    TPRINT(("Enter Mouse_hndlr\n"));
	    locx = stk_ptr->ecx & 0xFFFF;
	    locy = stk_ptr->edx & 0xFFFF;
	    MouseMove(locx, locy);
	    TrackCursor(locx, locy);
	    TPRINT(("Exit Mouse_hndlr\n"));
	    ScreenLocked=NIL;
	  }
      }

   return;
   }



set_DOSmouseposition(x, y)
  int x, y;
  { /* int 33h, case 0004, cx=col, dx = row */
    union REGS regs;
    regs.w.eax = 4;                    /* Function 4 = move cursor */
    regs.w.ecx = x;
    regs.w.edx = y;
    int86(0x33, &regs, &regs);

	/* Actually move the cursor image */

#ifndef OLD_CURSOR
    if(!ScreenLocked)
#endif
      { ScreenLocked=T;
	TPRINT(("Enter set_DOSmouseposition\n"));
	MouseMove(x, y);
	TrackCursor(x,y);
	TPRINT(("Enter set_DOSmouseposition\n"));
	ScreenLocked=NIL;
      }

  }


#endif /* DOS */
