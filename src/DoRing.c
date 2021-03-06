/* @(#) DoRing.c Version 1.5 (4/21/92). copyright venue & Fuji Xerox  */
static char *id = "@(#) DoRing.c	1.5 4/21/92	(venue & Fuji Xerox)";
/*
*
*
* Copyright (C) 1988 by Fuji Xerox co.,Ltd. All rights reserved.
*
*		Author: Mitsunori Matsuda
*		Date  : September 19, 1988
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
#include <sys/time.h>

#include "lispemul.h"
#include "keyboard.h"
#include "dbprint.h"


extern DLword *EmKbdAd068K
	    , *EmKbdAd168K
	    , *EmKbdAd268K
	    , *EmKbdAd368K
	    , *EmKbdAd468K
	    , *EmKbdAd568K
	    , *EmRealUtilin68K;
extern DLword *CTopKeyevent;
extern LispPTR *KEYBUFFERING68k;
extern int URaid_req
	 , KBDEventFlg;



/************************************************************************/
/*									*/
/*				D o R i n g				*/
/*									*/
/*	Take keyboard events and turn them into Lisp event info		*/
/*	(when running under X)						*/
/*									*/
/************************************************************************/

DoRing()
  {
    DLword w
         , r;
    KBEVENT *kbevent;

    TPRINT(( "TRACE: DoRing()\n" ));


do_ring:
    if(((*EmKbdAd268K) & 2113) == 0)
      { /*Ctrl-shift-NEXT*/
	error("******  EMERGENCY Interrupt ******");
	*EmKbdAd268K=KB_ALLUP; /*reset*/
	((RING*)CTopKeyevent)->read=0;  /* reset queue */
	((RING*)CTopKeyevent)->write=MINKEYEVENT; 
	/*return(0);*/
      } 
    else if(((*EmKbdAd268K) & 2114) == 0)
      { /* Ctrl-Shift-DEL */
	*EmKbdAd268K=KB_ALLUP; /*reset*/
	URaid_req=T;
	((RING*)CTopKeyevent)->read=0;  /* reset queue */
	((RING*)CTopKeyevent)->write=MINKEYEVENT; 
	/*return(0);*/
      }
	     
#ifdef OS4_TYPE4BUG
    else if(((*EmKbdAd268K) & 2120) == 0)
      { /* Ctrl-Shift-Return */
	*EmKbdAd268K=KB_ALLUP; /*reset*/
	URaid_req=T;
	((RING*)CTopKeyevent)->read=0;  /* reset queue */
	((RING*)CTopKeyevent)->write=MINKEYEVENT; 
      }
#endif

    r=RING_READ(CTopKeyevent);
    w=RING_WRITE(CTopKeyevent);

    if(r==w) /* event queqe FULL */
      goto KBnext;

    kbevent=(KBEVENT*)(CTopKeyevent+ w);
/*    RCLK(kbevent->time); */
    kbevent->W0= *EmKbdAd068K;
    kbevent->W1= *EmKbdAd168K;
    kbevent->W2= *EmKbdAd268K;
    kbevent->W3= *EmKbdAd368K;
    kbevent->W4= *EmKbdAd468K;
    kbevent->W5= *EmKbdAd568K;
    kbevent->WU= *EmRealUtilin68K;

    if(r==0) /* Queue was empty */
	((RING*)CTopKeyevent)->read=w;
    if(w >= MAXKEYEVENT) ((RING*)CTopKeyevent)->write = MINKEYEVENT;
    else ((RING*)CTopKeyevent)->write = w + KEYEVENTSIZE;

  KBnext:
    if(*KEYBUFFERING68k ==NIL) *KEYBUFFERING68k=ATOM_T;

  } 
