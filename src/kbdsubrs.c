/* @(#) kbdsubrs.c Version 1.4 (4/21/92). copyright venue & Fuji Xerox  */
static char *id = "@(#) kbdsubrs.c	1.4 4/21/92	(venue & Fuji Xerox)";


/************************************************************************/
/*									*/
/*	Copyright 1989, 1990 Venue, Fuji Xerox Co., Ltd, Xerox Corp.	*/
/*									*/
/*	This file is work-product resulting from the Xerox/Venue	*/
/*	Agreement dated 18-August-1989 for support of Medley.		*/
/*									*/
/************************************************************************/


/*
 *	Copyright (C) 1987 by Fuji Xerox Co., Ltd. All rights reserved.
 *
 *	Author	:	Osamu Nakamura
 */
#include <stdio.h>
#ifdef DOS
#include	<time.h>
#include <conio.h>
#else
#include <sys/time.h>
#include <sys/types.h>
#include <sys/file.h>
#endif /* DOS */

#ifdef SUNDISPLAY
#include <sundev/kbd.h>
#include <sundev/kbio.h>
#include <sunwindow/window_hs.h>
#endif /* SUNDISPLAY */

#ifdef DOS
#define PORT_A			0x60
#define KBD_COMMAND_PORT	0x64
#define KBD_ENABLE		0xAE
#define KBD_DISABLE		0xAD
#endif /* DOS */

#include "lispemul.h"

/****************************************************
 *
 *	KB_enable() entry of SUBRCALL 82 1
 *			called from (\KB_enable X)
 *
 ****************************************************/

#ifdef XWINDOW
#include <X11/Xlib.h>
#include "dspif.h"
extern DspInterface currentdsp;
#else /* XWINDOW */
extern struct screen LispScreen;
#endif /* XWINDOW */


extern int LispWindowFd,
	   LispReadFds;

extern int errno;

KB_enable( args )
LispPTR	*args;		/* args[0] :	ON/OFF flag
			 *		T -- ON
			 *		NIL -- OFF
			 */
{
	if( args[0] == ATOM_T )
#ifdef SUNDISPLAY
		LispReadFds |= 1<< LispWindowFd;
#endif /* SUNDISPLAY */

#ifdef XWINDOW
		enable_Xkeyboard(currentdsp);
#endif /* XWINDOW */

#ifdef DOS
	outp( KBD_COMMAND_PORT, KBD_ENABLE);
#endif /* DOS */

	else if( args[0] == NIL )
#ifdef SUNDISPLAY
		LispReadFds &= ~(1 << LispWindowFd);
#endif /* SUNDISPLAY */

#ifdef XWINDOW
		disable_Xkeyboard(currentdsp);
#endif /* XWINDOW */

#ifdef DOS
	outp( KBD_COMMAND_PORT, KBD_DISABLE);
#endif /* DOS */


	else{
		error("KB_enable: illegal arg \n");
		printf("KB_enable: arg = %d\n", args[0] );
	}
}



/****************************************************
 *
 *	KB_beep() entry of SUBRCALL 80 2
 *			called from (\KB_beep SW FREQ)
 *
 ****************************************************/
/*
struct timeval belltime ={
	0,100
};
*/
extern int LispKbdFd;

#ifdef DOS
  int bell_status_word;
#endif /* DOS */

KB_beep( args )
LispPTR	*args;		/* args[0] :	ON/OFF flag
			 *		T -- ON
			 *		NIL -- OFF
			 * args[1] :	frequency
			 */
{
  int keycommand;

#ifdef SUNDISPLAY
  /*	belltime.tv_usec = args[1] & 0xffff;
	win_bell(LispWindowFd, belltime, 0);
	*/
  if( (LispKbdFd = open( LispScreen.scr_kbdname, O_RDWR)) == -1)
    fprintf( stderr, "can't open %s, errno=%d\n",
	    LispScreen.scr_kbdname, errno);

  if( args[0] == ATOM_T ){
    keycommand = KBD_CMD_BELL;	/* Turn on the bell */
    if(ioctl( LispKbdFd, KIOCCMD, &keycommand)== -1)
      fprintf( stderr, "Error at ioctl errno =%d\n", errno);
  }
  else{
    keycommand = KBD_CMD_NOBELL; /* Turn off the bell */
    if(ioctl( LispKbdFd, KIOCCMD, &keycommand)== -1)
      fprintf( stderr, "Error at ioctl errno =%d\n", errno);
  }

  close( LispKbdFd );

#elif XWINDOW
  if( args[0] == ATOM_T ) 
    beep_Xkeyboard( currentdsp );

#elif DOS
  if( args[0] == ATOM_T ){
    bell_status_word = inp( 0x61 );
    outp( 0x61, bell_status_word | 0x3 ); /* Turn on the speaker */
    /* Prepare timer by sending 10111100 to port 43. */
    outp( 0x43, 0xb6 );

    /* Divide input frequency by timer ticks per second and
     * write (byte by byte) to timer. */
    outp( 0x42, (char)(1193180L / (LispIntToCInt(args[1]))) );
    outp( 0x42, (char)(1193180L / (LispIntToCInt(args[1])) >> 8) );
  } else {
    outp( 0x61, bell_status_word & ~0x3 ); /* Turn off the speaker (with */
    /* bits 0 and 1). */

  }

#endif				/* SUNDISPLAY XWINDOW DOS  */
}



/****************************************************
 *
 *	KB_setled()
 * Set the status LED's on the kbd.
 * arg[0] Caps lock LED.
 * arg[1] Num lock LED.
 * arg[2] Scroll lock LED.
 * NIL -> LED off
 * not NIL -> LED on.
 *
 ****************************************************/

KB_setled( args )
LispPTR	*args;
{
#ifdef DOS
  outp(PORT_A, (unsigned char)0xED);
  outp(PORT_A, (unsigned char)(((args[0] != NIL) << 2) |
			       ((args[1] != NIL) << 1) |
			       (args[2] != NIL)));
#endif /* DOS */
}

