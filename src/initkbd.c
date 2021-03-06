/* @(#) initkbd.c Version 1.17 (4/21/92). copyright venue & Fuji Xerox  */
static char *id = "@(#) initkbd.c	1.17 4/21/92	(venue & Fuji Xerox)";



/************************************************************************/
/*									*/
/*	(C) Copyright 1989, 1990, 1991 Venue. All Rights Reserved.	*/
/*	Manufactured in the United States of America.			*/
/*									*/
/*	The contents of this file are proprietary information 		*/
/*	belonging to Venue, and are provided to you under license.	*/
/*	They may not be further distributed or disclosed to third	*/
/*	parties without the specific permission of Venue.		*/
/*									*/
/************************************************************************/


#include <stdio.h>
#include <sys/types.h>
#ifndef DOS
#include <sys/file.h>
#endif /* DOS */
#ifdef SUNDISPLAY
#include <sundev/kbd.h>
#include <sundev/kbio.h>
#endif /* SUNDISPLAY */

#ifdef DOS
#include <i32.h>                       /* "#pragma interrupt" & '_chain_intr'*/
#include <dos.h>                       /* defines REGS & other structs       */
#include <stdio.h>                     /* define NULL                        */
#include <conio.h>
#include <time.h>
#include <stk.h>
#endif /* DOS */

#include <fcntl.h>
#ifdef SUNDISPLAY
#ifndef DOS
#include <sunwindow/window_hs.h>
#include <sunwindow/cms.h>
#include <sys/ioctl.h>
#include <sunwindow/win_ioctl.h>
#include <pixrect/pixrect_hs.h>
#endif /* DOS */
#endif /* SUNDISPLAY */

#ifdef DOS
#define KBD_COMMAND_PORT	0x64
#define KBD_ENABLE		0xAE
#define KBD_DISABLE		0xAD
#define KBD_RESET		0xF6

#define PORT_8042		0x60
#define KBD_SCAN_CODE_PORT	0x60

#define KBD_resend		0xFE
#define KBD_ack			0xFA
#define KBD_echo_responce	0xEE
#define KBD_failiure		0xFD
#define KBD_prefix		0xE0
#define KBD_pause_prefix	0xE1
#define KBD_overflow		0x00

#define INTA00			0x20 /* The 8259 port */
#define ENDOFINTERRUPT		0x20

#define PRTSC_KEY	0x37
#define HOME_KEY	0x47
#define UPARROW_KEY	0x48
#define PGUP_KEY	0x49
#define LEFTARROW_KEY	0x4b
#define RIGHTARROW_KEY	0x4d
#define END_KEY		0x4f
#define DOWNARROW_KEY	0x50
#define PGDOWN_KEY	0x51
#define INS_KEY		0x52
#define DEL_KEY		0x53

void      (*prev_int_09)();	/* keeps address of previous 09 handlr*/
#pragma interrupt(DOSkbd)
void  DOSkbd(void);		/* the flih-kbd handler INT 0x09   */
unsigned char inchar = 0;

extern int KBDEventFlg;
#endif /* DOS */

#ifdef XWINDOW
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include "XKeymap.h"
#include "dspif.h"
extern DspInterface currentdsp;
#endif /* XWINDOW */


#ifdef ISC
#include <sys/bsdtypes.h>
#endif /* ISC */


#include "lispemul.h"
#include "lispmap.h"
#include "lspglob.h"
#include "adr68k.h"
#include "address.h"

#include "devconf.h"
#include "iopage.h"
#include "ifpage.h"
#include "keyboard.h"

#ifdef SUNDISPLAY
extern struct screen LispScreen;
#endif /* SUNDISPLAY */

extern int LispWindowFd;
int LispKbdFd;
extern int errno;

/*   for debug    */
int	DebugKBD = NIL;
FILE	*KBlog;

u_int LispReadFds = 0;
#ifdef SUNDISPLAY
struct inputmask LispEventMask;
#endif /* SUNDISPLAY */


IOPAGE	*IOPage68K;

DLword	*EmMouseX68K;
DLword	*EmMouseY68K;
DLword	*EmCursorX68K;
DLword	*EmCursorY68K;
DLword	*EmRealUtilin68K;
DLword	*EmUtilin68K;
DLword	*EmKbdAd068K;
DLword	*EmKbdAd168K;
DLword	*EmKbdAd268K;
DLword	*EmKbdAd368K;
DLword	*EmKbdAd468K;
DLword	*EmKbdAd568K;
DLword	*EmDispInterrupt68K;
DLword	*EmCursorBitMap68K;

/*u_char SUNLispKeyMap[128];*/
u_char *SUNLispKeyMap;

/* keymap for type3 */
u_char SUNLispKeyMap_for3[128] = {

/* 0 */	  -1,  61,  -1,  91,  -1,  97,  99,  -1,
/* 8 */  100,  -1,  67,  -1,  68,  -1, 101,  -1,
/* 16*/   66, 104,  80,  47,  -1,  73,  74,  75,
/* 24*/   -1,  92,  63,  -1,  -1,  33,  32,  17,
/* 32*/   16,   1,   0,   2,   4,  53,  22,   8,
/* 40*/	  10,  59,  45,  13,  -1,  81,  82,  83,
/* 48*/   -1,  14,  -1,  62,  -1,  34,  19,  18,
/* 56*/    3,  48,  49,  51,   6,  23,  25,  11,
/* 64*/   58,  29,  15,  -1,  84,  85,  87,  -1,
/* 72*/  111,  89,  -1,  -1,  36,  21,  20,   5,
/* 80*/	  35,  50,  52,  38,   9,  26,  43,  28,
/* 88*/  105,  44,  -1,  94,  69,  70,  -1,  90,
/* 96*/   -1,  46,  -1,  41,  40,  24,  37,   7,
/*104*/   39,  54,  55,  27,  42,  12,  60,  71,
/*112*/   98,  76,  72,  -1,  -1,  -1,  -1,  56,
/*120*/	  31,  57,  93,  -1,  -1,  -1,  -1,  -1,
 };

/* for type4 */

u_char SUNLispKeyMap_for4[128] = {

/* 0 */   -1,  61,  -1,  91,  -1,  97,  99, 106,
/* 8 */  100, 107,  67, 108,  68,  47, 101,  30,
/* 16*/   66, 104,  80,  31,  -1,  75, 110,  74,
/* 24*/   -1, 109,  63,  -1,  -1,  33,  32,  17,
/* 32*/   16,   1,   0,   2,   4,  53,  22,   8,
/* 40*/   10,  59,  45,  15,  -1,  64,  65,  95,
/* 48*/   -1,  14,  13,  89,  -1,  34,  19,  18,
/* 56*/    3,  48,  49,  51,   6,  23,  25,  11,
/* 64*/   58,  29,  13,  93,  81,  82,  83,  96,
/* 72*/  111,  62,  -1,  -1,  36,  21,  20,   5,
/* 80*/   35,  50,  52,  38,   9,  26,  43,  28,
/* 88*/  105,  44,  76,  84,  85,  87,  98,  90,
/* 96*/   -1,  46,  73,  41,  40,  24,  37,   7,
/*104*/   39,  54,  55,  27,  42,  12,  60,  71,
/*112*/   94,  69,  70,  -1,  -1,  -1,  92,  56,
/*120*/   86,  57,  88,  -1, 103, 102,  -1,  -1,
 };

/* for jle */

u_char SUNLispKeyMap_jle[128] = {

/* 0 */   -1,  61,  -1,  91,  -1,  97,  99, 106,
/* 8 */  100, 107,  67, 108,  68,  47, 101,  71,
/* 16*/   66, 104,  80,  31,  -1,  75, 110,  74,
/* 24*/   -1, 109,  63,  -1,  -1,  33,  32,  17,
/* 32*/   16,   1,   0,   2,   4,  53,  22,   8,
/* 40*/   59,  45,  30,  15,  -1,  64,  65,  95,
/* 48*/   -1,  14,  13,  89,  -1,  34,  19,  18,
/* 56*/    3,  48,  49,  51,   6,  23,  25,  11,
/* 64*/   58,  29,  13,  93,  81,  82,  83,  96,
/* 72*/  111,  62,  -1,  -1,  36,  21,  20,   5,
/* 80*/   35,  50,  52,  38,   9,  26,  43,  28,
/* 88*/  105,  44,  76,  84,  85,  87,  98,  90,
/* 96*/   -1,  46,  73,  41,  40,  24,  37,   7,
/*104*/   39,  54,  55,  27,  42,  12,  60,  10,
/*112*/   94,  69,  70,  72, 103, 109,  92,  56,
/*120*/   86,  57,  88,  -1,  -1, 102,  -1,  -1,
 };
/* [40] 10->59  */
/* [41] 59->45  */
/* [42] 45->30  */
/* [111] 71->10 */
/* [115] -1 -> 72  Kakutei */
/* [116] -1 -> 103 Henkan */
/* [117] -1 -> 109 Nihongo On-Off */



/* For IBM Risc System 6000 */

u_char SUNLispKeyMap_RS6000[128] = {

/* 7 */   -1, -1,   45,  32,  17,  16,   1,   0,
/* 15*/    2,   4,  53,  22,   8,  10,  59,  -1,
/* 23*/   15,  34,  19,  18,   3,  48,  49,  51,
/* 31*/    6,  23,  25,  11,  58,  29, 105,  56,
/* 39*/   21,  20,   5,  35,  50,  52,  38,   9,
/* 47*/   26,  43,  28,  -1,  44,  41,  -1,  40,
/* 55*/   24,  37,   7,  39,  54,  55,  27,  42,
/* 63*/   12,  -1,  60,  36,  -1,  31,  57,  93,
/* 71*/   -1,  47,  -1,  -1,  -1,  -1,  -1,  -1,
/* 79*/   -1,  -1,  -1,  -1,  89,  46,  -1,  -1,
/* 87*/  129,  62,  90,  -1, 130, 131,  63,  91,
/* 95*/   -1,  -1, 132,  73,  81,  84,  94,  -1,
/*103*/   65,  82,  85,  69,  98,  95,  83,  87,
/*111*/   70,  13,  96, 102,  -1, 128,  -1,  33,
/*119*/   -1,  97,  99, 100,  67,  68, 101,  66,
/*127*/  104,  80, 106, 107,  92,  76,  14,  61,
/*136*/ 
 };



/* For DECStation 3100 */

u_char SUNLispKeyMap_DEC3100[256] = {

/*  7*/  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
/* 15*/  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
/* 23*/  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
/* 31*/  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
/* 39*/  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
/* 47*/  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
/* 55*/  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
/* 63*/  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
/* 71*/  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
/* 79*/  -1,  -1,  -1,  -1,  -1,  -1,  -1,  97,
/* 87*/  99, 100,  67,  68,  -1,  -1,  -1,  -1,
/* 95*/  -1,  -1,  -1,  -1,  -1, 101,  66, 104,
/*103*/  80, 106,  -1,  -1,  -1,  -1,  -1,  -1,
/*111*/  -1,  -1, 107, 108,  -1,  -1,  -1,  -1,
/*119*/  -1,  -1,  -1,  -1,  -1,  92,  76,  -1,
/*127*/  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
/*135*/  -1,  -1,  -1,  90,  91,  46,  93,  89,
/*143*/  47,  -1,  -1,  98,  -1,  13,  76,  94,
/*151*/  69,  70,  84,  85,  87, 102,  81,  82,
/*159*/  83,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
/*167*/ 129, 132, 131, 130,  41,  -1,  -1,  41,
/*175*/  36,  56,  31,  -1,  -1,  -1,  -1,  -1,
/*183*/  -1,  -1,  -1,  -1,  -1,  15,  44,  34,
/*191*/  33,  32,  19,  21,  40,  -1,  17,  18,
/*199*/  20,  24,  45,  -1,  16,   3,   5,  37,
/*207*/  -1,   1,  48,  35,   7,  57,  -1,   0,
/*215*/  49,  50,  39,  -1,   2,  51,  52,  54,
/*223*/  -1,   4,   6,  38,  55,  -1,  53,  23,
/*231*/   9,  27,  -1,  22,  25,  26,  42,  -1,
/*239*/   8,  11,  -1,  43,  12,  -1,  59,  29,
/*247*/ 105,  -1,  10,  58,  28,  -1,  -1,  -1 

  };


u_char SUNLispKeyMap_HP9000[135] = 
  {
/*   7*/  -1, -1, -1, 93, 31, 60, 41, 36,
/*  15*/  -1, -1, -1, -1, -1, -1, -1, -1,
/*  23*/  -1, -1, -1, -1, -1, -1, -1, -1,
/*  31*/  -1, 39,  7, 37, 24, 40, -1, -1,
/*  39*/  -1, -1, -1, -1, -1, -1, -1, -1,
/*  47*/  -1, 52, 50, 35,  5, 20, 21, -1,
/*  55*/  56,  6, 51, 49, 48,  3, 18, 19,
/*  63*/  34,  4,  2,  0,  1, 16, 17, 32,
/*  71*/  45, -1, -1, -1, -1, -1, -1, -1,
/*  79*/  -1, 68, 67,100, 99, 97, -1, -1,
/*  87*/  -1, 68,101, 66,104, 80, 13, -1,
/*  95*/  -1, 53, 22,  8, 10, 59, 15, -1,
/* 103*/  -1, 23, 25, 11, 58, 29,105, -1,
/* 111*/  -1, 38,  9, 26, 43, 28, 44, -1,
/* 119*/  -1, 55, 27, 42, 12, -1, -1, -1,
/* 127*/  47, 54, 57, -1, -1, -1, -1, -1
};

u_char *XGenericKeyMap;	/* filled in with malloc if needed */


/* For the IBM-101 kbd FF marks exceptions */

#ifdef NEVER
u_char DOSLispKeyMap_101[0x80] =
{
/*         0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f */

/* 0*/  0xff,  33,  32,  17,  16,   1,   0,   2,   4,  53,  22,   8,  10,  59,  15,  34,
/* 1*/    19,  18,   3,  48,  49,  51,   6,  23,  25,  11,  58,  29,  44,  36,  21,  20,
/* 2*/     5,  35,  50,  52,  38,   9,  26,  43,  28,  45,  41, 105,  40,  24,  37,   7,
/* 3*/    39,  54,  55,  27,  42,  12,  60,  95,  31,  57,  56,  97,  99, 100,  67,  68,
/* 4*/   101,  66, 104,  80, 106,  73,  74,  81,  82,  83,  96,  84,  85,  87, 102,  94,
/* 5*/    69,  70,  98,  13,0xff,0xff,0xff, 107, 108,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
/* 6*/    89,  62,  63,  46,  90,  91, 130, 129, 131, 132,  92,  61,0xff,0xff,0xff,0xff,
/* 7*/  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
};
#endif /* NEVER */

/* For the IBM-101 kbd FF marks exceptions */
u_char DOSLispKeyMap_101[0x80] =
{
/*         0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f */

/* 0*/  0xff,  33,  32,  17,  16,   1,   0,   2,   4,  53,  22,   8,  10,  59,  15,  34,
/* 1*/    19,  18,   3,  48,  49,  51,   6,  23,  25,  11,  58,  29,  44,  36,  21,  20,
/* 2*/     5,  35,  50,  52,  38,   9,  26,  43,  28,  45,  41, 105,  40,  24,  37,   7,
/* 3*/    39,  54,  55,  27,  42,  12,  60,  95,  31,  57,  56,  97,  99, 100,  67,  68,
/* 4*/   101,  66, 104,  80, 106,  73,  74,  62, 130,  63,  96, 129,  85, 132, 102,  90,
/* 5*/   131,  91,  89,  46,0xff,0xff,0xff, 107, 108,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
/* 6*/    89,  62,  63,  46,  90,  91, 130, 129, 131, 132,  92,  61,0xff,0xff,0xff,0xff,
/* 7*/  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
};


init_keyboard(flg)
  int flg ; /* if 0 init else re-init */
  {
    int keytrans;

    set_kbd_iopointers();

#ifdef SUNDISPLAY
    if( (LispKbdFd = open( LispScreen.scr_kbdname , O_RDWR)) == -1)
      {
	fprintf( stderr, "can't open %s\n", LispScreen.scr_kbdname);
	exit( -1 );
      }
#endif				/* SUNDISPLAY */

    if(flg==0)
      {
	keyboardtype(LispKbdFd);
      }

#ifdef SUNDISPLAY
    keytrans = TR_UNTRANS_EVENT; /* keyboard does not encode key */
    if(ioctl( LispKbdFd, KIOCTRANS, &keytrans)== -1)
      {
	fprintf( stderr, "Error at ioctl errnoo =%d\n", errno);
	exit( -1 );
      }
    close( LispKbdFd );

    seteventmask( &LispEventMask );

#elif XWINDOW
    init_Xevent(currentdsp);

#elif DOS
    if (flg == 0)
      {		/* Install the handlers ONLY when we */
		/* init the kbd the init the kbd the */
		/* first time. */
	prev_int_09 = _dos_getvect(0x09); /* get addr of currnt 09 hndlr */
      	_dos_setvect(0x09, DOSkbd); /* hook our int handler to interrupt  */

	init_DOSmouse(); /* turn on mouse */
    }
    /*    outp( KBD_COMMAND_PORT, KBD_RESET); Reset the kbd */
#endif				/* XWINDOW DOS */
  }




/*  ----------------------------------------------------------------*/

device_before_exit()
{
#ifdef SUNDISPLAY
	int keytrans;

	if( (LispKbdFd = open( LispScreen.scr_kbdname , O_RDWR)) == -1){
		fprintf( stderr, "can't open %s\n", LispScreen.scr_kbdname);
		exit( -1 );
	}

	keytrans = TR_EVENT;	/* keyboard encodes key */
	if(ioctl( LispKbdFd, KIOCTRANS, &keytrans)== -1){
		fprintf( stderr, "Error at ioctl errnoo =%d\n", errno);
			exit( -1 );
	}
	close( LispKbdFd );

#elif DOS
  /* Restore the kbd */
  _dos_setvect(0x09, prev_int_09); /* unhook our handlr, install previous*/

#endif  /* SUNDISPLAY DOS*/
	display_before_exit();
}




/*  ----------------------------------------------------------------*/

set_kbd_iopointers()
{
	IOPage68K       = (IOPAGE *)IOPage;
	EmMouseX68K     = (DLword *) &(IOPage68K->dlmousex);
	EmMouseY68K     = (DLword *) &(IOPage68K->dlmousey);
	EmCursorX68K    = (DLword *) &(IOPage68K->dlcursorx);
	EmCursorY68K    = (DLword *) &(IOPage68K->dlcursory);
	EmRealUtilin68K = (DLword *) &(IOPage68K->dlutilin);
	/* EmUtilin68K is for KEYDOWNP1 macro or etc. */
	EmUtilin68K = (DLword *) &(InterfacePage->fakemousebits);
	EmKbdAd068K = (DLword *) &(IOPage68K->dlkbdad0);
	EmKbdAd168K = (DLword *) &(IOPage68K->dlkbdad1);
	EmKbdAd268K = (DLword *) &(IOPage68K->dlkbdad2);
	EmKbdAd368K = (DLword *) &(IOPage68K->dlkbdad3);
	EmKbdAd468K = (DLword *) &(IOPage68K->dlkbdad4);
	EmKbdAd568K = (DLword *) &(IOPage68K->dlkbdad5);
	EmDispInterrupt68K = (DLword *) &(IOPage68K->dldispinterrupt);
	EmCursorBitMap68K  = (DLword *)  (IOPage68K->dlcursorbitmap);

    *EmRealUtilin68K = KB_ALLUP;
    *EmKbdAd068K = KB_ALLUP;
    *EmKbdAd168K = KB_ALLUP;
    *EmKbdAd268K = KB_ALLUP;
    *EmKbdAd368K = KB_ALLUP;
    *EmKbdAd468K = KB_ALLUP;
    *EmKbdAd568K = KB_ALLUP;

}




/*  ----------------------------------------------------------------*/

#ifdef SUNDISPLAY
seteventmask( eventmask )
struct inputmask *eventmask;
{
	input_imnull( eventmask );
	eventmask->im_flags  |=  IM_ASCII | IM_NEGASCII | IM_NEGEVENT;

	win_setinputcodebit  ( eventmask, MS_LEFT );
	win_setinputcodebit  ( eventmask, MS_MIDDLE );
	win_setinputcodebit  ( eventmask, MS_RIGHT );
	win_setinputcodebit  ( eventmask, LOC_MOVE );
	win_unsetinputcodebit( eventmask, LOC_STILL );
	win_unsetinputcodebit( eventmask, LOC_MOVEWHILEBUTDOWN );
	win_unsetinputcodebit( eventmask, LOC_WINENTER );
	win_unsetinputcodebit( eventmask, LOC_WINEXIT );

	win_setinputmask( LispWindowFd, eventmask, eventmask, WIN_NULLLINK);
}
#endif /* SUNDISPLAY */



char* getenv();		/*  ---- external entry points --------*/

#define MIN_KEYTYPE 3
#define KB_AS3000J (7 + MIN_KEYTYPE)
#define KB_RS6000  (8 + MIN_KEYTYPE)
#define KB_DEC3100 (9 + MIN_KEYTYPE)
#define KB_HP9000  (10+ MIN_KEYTYPE)
#define KB_X	   (11+ MIN_KEYTYPE)
#define KB_DOS	(12+MIN_KEYTYPE)

/* KB_SUN4 not defined in older OS versions */
#ifndef KB_SUN4
#define KB_SUN4 4
#endif

#ifndef KB_SUN2
	/* These KB types nog defined outside Sun world,so define them here */
#define KB_SUN2 2
#define KB_SUN3 3
#endif /* XWINDOW */


/* For the JLE keyboard */
#define KB_JLE 5



/************************************************************************/
/*									*/
/*			m a k e _ X _ k e y m a p			*/
/*									*/
/*	Starting from the generic-X-keyboard mapping in XKeymap.h,	*/
/*	construct a keyboard map for this machine, using the rules	*/
/*	shown in the header file.					*/
/*									*/
/*									*/
/************************************************************************/

#ifdef XWINDOW
u_char * make_X_keymap()
  {
    u_char *table = (u_char *)malloc(256);	/* the final result table */
    int lisp_codes_used[256];	/* Keep track of the Lisk key #s we've used */
    int last_KEYSYM = -1;
    int sym_used = 0;
    int *key_sym_pairs = generic_X_keymap;
    int i = 0;

    for (; i<256; i++)
      { /* clear the tables we just allocated */
	lisp_codes_used[i] = 0;
	table[i]=255;		/* The "no key assigned" code */
      }

    for (; *key_sym_pairs != -1; )
      {
	int code = *key_sym_pairs++,
	    sym  = *key_sym_pairs++,
	    xcode;

	if (sym_used && (sym == last_KEYSYM)) continue;

	sym_used = 0;
	last_KEYSYM = sym;


	xcode = XKeysymToKeycode(currentdsp->display_id, sym);

	if ((xcode==0) || (table[xcode-7] != 255)) continue;
	if (lisp_codes_used[code] != 0) continue;
	
	sym_used = 1;
	last_KEYSYM = sym;
	lisp_codes_used[code] = 1;
	table[xcode-7] = code;

	
      }

#ifdef DEBUG
    for (i=0; i < 256; i += 8)
      {
	printf("\n\n\tKeyboard mapping table\n\n");
	printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
		table[i], table[i+1], table[i+2], table[i+3],
		table[i+4], table[i+5], table[i+6], table[i+7]);
      }
#endif /* DEBUG */


    return(table);
  }
#endif /* XWINDOW */





/************************************************************************/
/*									*/
/*			  k e y b o a r d t y p e			*/
/*									*/
/*	Determine what kind of keyboard we're dealing with, by		*/
/*	checking the LDEKBDTYPE shell variable.  It it's not set,	*/
/*	either default it (for DEC, IBM), or complain and exit.		*/
/*	Valid LDEKBDTYPE values:					*/
/*		type3	Sun type-3 keyboard				*/
/*		type4	Sun type-4 keyboard				*/
/*		rs6000	IBM RS/6000					*/
/*		hp9000	HP 9000 series 800 or 700			*/
/*		dec3100	DECstation 3100 or 5000				*/
/*		x	generic X keyboard map				*/
/*									*/
/*									*/
/*									*/
/*									*/
/*									*/
/*									*/
/************************************************************************/

keyboardtype(fd)
  int fd;
  {
    int type;
    int i;
    char *key;

    for(i=0;i<5000;i++){/* IDLE LOOP */} /* because of a SunOS bug */

    /* clear the keyboard field in devconfig */
    InterfacePage->devconfig &= 0xfff8;

   /************************************************************
    Due to the problems of SunOS 4.0 & 4.0.1
    calling ioctl never return the correct keyboard type.
    So,these 2 lines are commented out ...(Take)->AR11100
   *************************************************************/

    /* Get keytype from LDEKBDTYPE  */
    if ((key = getenv("LDEKBDTYPE")) == 0) {
#ifdef FUJI
	/* obnoxious behavior */
	mess_reset();
	printf("!!CAUTION: LDEKBDTYPE is not set.\n");
	printf("Please retry after setting LDEKBDTYPE correctly.\n");
	printf("Usage: setenv LDEKBDTYPE <kbdtype>\n");
	printf("       (one of type2, type3, type4, jle, or as3000j)");
	exit(0); /* exit to shell */
#else
#ifdef DEC3100
	type = KB_DEC3100;
#else
#ifdef RS6000
	type = KB_RS6000;
#else
#ifdef HP9000
	type = KB_HP9000;
#else
#ifdef XWINDOW
	type = KB_X;
#elif DOS
	type = KB_DOS;
#else
  	if(ioctl(fd,KIOCTYPE,&type) !=0) {
		error("keyboardtype:IOCTL(KIOCTYPE) fails (cont. w. type-3");
		type = KB_SUN3;
		}	/* otherwise, type is set */
#endif /* XWINDOW */

#endif /* HP9000 */

#endif /* RS6000 */

#endif /* DEC3100 */

#endif /* FUJI */

#ifdef XWINDOW
#undef FUJI
#endif /* XWINDOW */

 }/* if end */
 else {
	if      (strcmp("as3000j", key)==0) type = KB_AS3000J;
	else if (strcmp("type4",   key)==0) type = KB_SUN4;
	else if (strcmp("type2",   key)==0) type = KB_SUN2;
        else if (strcmp("jle",     key)==0) type = KB_JLE;
	else if (strcmp("rs6000",  key)==0) type = KB_RS6000;
	else if (strcmp("dec3100", key)==0) type = KB_DEC3100;
	else if (strcmp("hp9000",  key)==0) type = KB_HP9000;
	else if (strcmp("X",       key)==0) type = KB_X;
	else if (strcmp("x",       key)==0) type = KB_X;
	else     type = KB_SUN3;           /* default */
       }


 switch(type) {
	case KB_SUN2 : /* type2, we still use keymap for type3 */
		SUNLispKeyMap = SUNLispKeyMap_for3;
		/* MIN_KEYTYPE is 3,so we can't set devconfig correctly */
		/* Therefore type2 may treat as type3 */
		InterfacePage->devconfig |= 0;
		break;
	case KB_SUN3 : /* type3 */
		SUNLispKeyMap = SUNLispKeyMap_for3;
		InterfacePage->devconfig |= type-MIN_KEYTYPE;
		break;
	case KB_SUN4 : /* type4 */
		SUNLispKeyMap = SUNLispKeyMap_for4;
		InterfacePage->devconfig |= type-MIN_KEYTYPE;
		break;
	case KB_JLE : /* JLE */
		/*printf("jle\n"); */
		SUNLispKeyMap=SUNLispKeyMap_jle;
		InterfacePage->devconfig |= type-MIN_KEYTYPE;
		/* InterfacePage->devconfig |= 4-MIN_KEYTYPE; */
		break;
	case KB_AS3000J : /* for AS, use type4 map */
		SUNLispKeyMap = SUNLispKeyMap_for4;
		InterfacePage->devconfig |= type-MIN_KEYTYPE;/* 7 */
		break;
	case KB_RS6000:
		SUNLispKeyMap = SUNLispKeyMap_RS6000;
		InterfacePage->devconfig |= KB_SUN3 - MIN_KEYTYPE; /* 8 */
		break;
	case KB_DEC3100:
		SUNLispKeyMap = SUNLispKeyMap_DEC3100;
		InterfacePage->devconfig |= KB_SUN3 - MIN_KEYTYPE; /* 9 */
		break;
	case KB_HP9000:
		SUNLispKeyMap = SUNLispKeyMap_HP9000;
		InterfacePage->devconfig |= KB_SUN3 - MIN_KEYTYPE; /* 10 */
		break;
#ifdef XWINDOW
	case KB_X:
		XGenericKeyMap = (u_char *)make_X_keymap();
		SUNLispKeyMap = XGenericKeyMap;
		InterfacePage->devconfig |= KB_SUN3 - MIN_KEYTYPE; /* 10 */
		break;
#endif /* XWINDOW */

#ifdef DOS
	case KB_DOS:
		SUNLispKeyMap = DOSLispKeyMap_101;
		InterfacePage->devconfig |= KB_SUN3 - MIN_KEYTYPE; /* 10 */
		break;
#endif /* DOS */
	default: { char errmsg [ 200 ];
		sprintf ( errmsg, "Unsupported keyboard type: %d", type);
                printf ( "%s\n", errmsg);
                printf ( "Configuring keyboard for type-3\n", errmsg);
                SUNLispKeyMap = SUNLispKeyMap_for3;
                InterfacePage->devconfig |= KB_SUN3-MIN_KEYTYPE;
		break;
		}
	}

}/* end keyboardtype*/

#ifdef DOS
unsigned short kn;

void DOSkbd()
{
  
  _XSTACK *ebp;			/* Real-mode handler stack frame */

  ebp = (_XSTACK *) _get_stk_frame(); /* Get stack frame address */
  ebp->opts |= _STK_NOINT;	/* Bypass real-mode handler */

  outp( KBD_COMMAND_PORT, KBD_DISABLE);	/* Turn the kbd off. */

  inchar = inp( KBD_SCAN_CODE_PORT ); /* Read the char from the kbd */
  outp (INTA00, ENDOFINTERRUPT);

  switch ( inchar ) {
  case KBD_overflow :
    break;
  case KBD_prefix :		/* It's a prefix, Try to untangle it. */
    outp (INTA00, ENDOFINTERRUPT);
    switch (inp( KBD_SCAN_CODE_PORT )) {
    case PRTSC_KEY :		inchar = 0x6A;
      break;
    case HOME_KEY :		inchar = 0x47;
      break;
    case UPARROW_KEY :		inchar = 0x48;
      break;
    case PGUP_KEY :		inchar = 0x49;
      break;
    case LEFTARROW_KEY :	inchar = 0x4b;
      break;
    case RIGHTARROW_KEY :	inchar = 0x4d;
      break;
    case END_KEY :		inchar = 0x4f;
      break;
    case DOWNARROW_KEY :	inchar = 0x50;
      break;
    case PGDOWN_KEY :		inchar = 0x51;
      break;
    case INS_KEY :		inchar = 0x52;
      break;
    case DEL_KEY :		inchar = 0x53;
      break;
    case 0x2A :			/* These are just misc. shift states. Ignore. */
    case 0x36 :
    case 0xAA :
    case 0xB6 :
      break;
    }
    break;
  case KBD_pause_prefix :	inchar = 0x6B;
    break;
  case KBD_echo_responce :
    break;
  case KBD_ack :		/* It's an ack, flag that it has been found */
    inchar = 0;
    outp (INTA00, ENDOFINTERRUPT);
    break;
  case KBD_failiure :
    break;
  case KBD_resend :
    break;
  default: kb_event(inchar);
    break;
  }
  
  outp( KBD_COMMAND_PORT, KBD_ENABLE); /* Finally turn that kbd on again. */
  return;
}
#endif /* DOS */
