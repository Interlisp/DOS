/* @(#) adr68k.h Version 1.1 (4/21/92). copyright Venue & Fuji Xerox  */

/*
 *	Copyright (C) 1987 by Fuji Xerox Co., Ltd. All rights reserved.
 *
 *	Author	:	Takeshi Shimizu
 *			Hiroshi Hayata
 */



/************************************************************************/
/*									*/
/*	Copyright 1989, 1990 Venue, Fuji Xerox Co., Ltd, Xerox Corp.	*/
/*									*/
/*	This file is work-product resulting from the Xerox/Venue	*/
/*	Agreement dated 18-August-1989 for support of Medley.		*/
/*									*/
/************************************************************************/



/**********************************************************************/
/*
		Func name :	adr68k.h
		Translate 68k address to Lisp or Lisp to 68k

		Date :		January 16, 1987
		Create :	Takeshi Shimizu
*/
/**********************************************************************/



/*  translate 68k ptr to Lisp DLword address */
#define LADDR_from_68k(ptr68k)	(((unsigned int)(ptr68k) - (unsigned int)Lisp_world) >>1)


/*  translate 68k ptr to Lisp Page number   */
#define LPAGE_from_68k(ptr68k)	(LADDR_from_68k(ptr68k) >> 8)


/* Translate Lisp_address to 68K address	*/
/*	Lisp_addr: word offset		*/
#ifndef I386
#define Addr68k_from_LADDR(Lisp_addr)	(Lisp_world + (Lisp_addr))
#else
#define Addr68k_from_LADDR(Lisp_addr)	((Lisp_addr) + Lisp_world)
#endif


/* translate LispPage to 68k address */
#define Addr68k_from_LPAGE(Lisp_page)	(Addr68k_from_LADDR((Lisp_page << 8) ))




/* Stack Offset Macros */

#define StkOffset_from_68K(ptr68k)\
	(((unsigned int)(ptr68k) - (unsigned int)Stackspace) >>1)

#ifndef I386
#define Addr68k_from_StkOffset(stkoffset)\
	(Stackspace + (stkoffset))
#else
#define Addr68k_from_StkOffset(stkoffset)\
	( (stkoffset) + Stackspace)
#endif
