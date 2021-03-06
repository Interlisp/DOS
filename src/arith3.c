/* @(#) arith3.c Version 2.6 (11/30/88). copyright envos & Fuji Xerox  */
static char *id = "@(#) arith3.c	2.6 11/30/88	(envos & Fuji Xerox)";



/************************************************************************/
/*									*/
/*	Copyright 1989, 1990 Venue, Fuji Xerox Co., Ltd, Xerox Corp.	*/
/*									*/
/*	This file is work-product resulting from the Xerox/Venue	*/
/*	Agreement dated 18-August-1989 for support of Medley.		*/
/*									*/
/************************************************************************/


/************************************************************************/
/*									*/
/*			    A R I T H 3 . C				*/
/*									*/
/*	Including :	OP_makenumber					*/
/*			OP_boxiplus					*/
/*			OP_boxidiff					*/
/*									*/
/*									*/
/************************************************************************/


#include "lispemul.h"
#include "lispmap.h"
#include "emlglob.h"
#include "lspglob.h"
#include "lsptypes.h"
#include "address.h"
#include "adr68k.h"
#include "cell.h"
#include "arith.h"




/************************************************************************/
/*									*/
/*			N _ O P _ m a k e n u m b e r			*/
/*									*/
/*	Given the 2 halves of a FIXP as SMALLP's, create a number	*/
/*	box for the number, and fill it in.				*/
/*									*/
/************************************************************************/

N_OP_makenumber(tosm1, tos)
	int tosm1, tos;
{
  register int result;

  if(((tosm1 & 0xFFFF0000) != S_POSITIVE) || ((tos & 0xFFFF0000) !=  S_POSITIVE))
	ERROR_EXIT(tos);
  result = ((tosm1 & 0xffff) << 16) | (tos & 0xffff);
  N_ARITH_SWITCH(result);
} /* end OP_makenumber */



/************************************************************************/
/*									*/
/*			N _ O P _ b o x i p l u s			*/
/*									*/
/*	Given a FIXP box and a number to add to it, add the number,	*/
/*	leaving the result in the box given.  Used to avoid garbaging.	*/
/*									*/
/************************************************************************/

N_OP_boxiplus(a, tos)
  register int a;
  int tos;
{
  register int    arg2;

  if( GetTypeNumber(a) == TYPE_FIXP )
   {
	N_GETNUMBER(tos, arg2, bad);
	*((LispPTR *)Addr68k_from_LADDR(a)) += arg2;
	return(a);
   }
bad:	ERROR_EXIT(tos);

} /* OP_boxiplus */



/************************************************************************/
/*									*/
/*			O P _ b o x i d i f f				*/
/*									*/
/*	Given a FIXP box and a number to subtract from it, do the	*/
/*	subtraction, and leave the result in the box given.		*/
/*	Used to avoid allocating storage in low-level routines.		*/
/*									*/
/************************************************************************/

N_OP_boxidiff(a, tos)
  register int a;
  int tos;
{
  register int    arg2;

  if( GetTypeNumber(a) == TYPE_FIXP )
   {
	N_GETNUMBER(tos, arg2, bad);
	*((LispPTR *)Addr68k_from_LADDR(a)) -=  arg2;
	return(a);
   }
  bad: ERROR_EXIT(tos);

} /* end OP_boxidiff */


