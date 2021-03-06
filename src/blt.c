/* @(#) blt.c Version 2.8 (4/21/92). copyright Venue & Fuji Xerox  */
static char *id = "@(#) blt.c	2.8 4/21/92";



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
 *	Author :  Takeshi Shimizu
 *
 */
/******************************************************************/
/*
		File Name  :	blt.c
		Including  :	OP_blt

		Created    :	jul 9, 1987 by T.Shimizu
*/
/******************************************************************/

#include "lispemul.h"
#include "address.h"
#include "adr68k.h"
#include "lsptypes.h"
#include "lispmap.h"
#include "stack.h"
#include "emlglob.h"
#include "lspglob.h"
#include "cell.h"

/*
  N_OP_blt takes 3 arguments.
	STK-1 has destination's pointer.
	STK has source's pointer.
	TOS has number of words to be translated.
*/


LispPTR N_OP_blt(destptr, sourceptr, wordcount)
  LispPTR destptr, sourceptr;
  register LispPTR wordcount;

  {
    register  DLword *source68k;
    register  DLword *dest68k ;
    register int nw;

    if((wordcount & 0xff0000) != S_POSITIVE) ERROR_EXIT(wordcount);
    nw = wordcount & 0xffff ;

    source68k = Addr68k_from_LADDR(sourceptr) + nw;
    dest68k = Addr68k_from_LADDR(destptr) + nw;

    while(nw)	{  (GETWORD(--dest68k))= GETWORD(--source68k) ; nw--; }

    return(wordcount);
  } /* end N_OP_blt */




