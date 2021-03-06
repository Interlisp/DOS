/* @(#) typeof.c Version 2.13 (4/21/92). copyright Venue & Fuji Xerox  */
static char *id = "@(#) typeof.c	2.13 4/21/92";
/*
 *	Copyright (C) 1987 by Fuji Xerox Co., Ltd. All rights reserved.
 *
 *	File   : typeof.c
 *	Author :  Takeshi Shimizu

	** ADOPTED NEW VERSION

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




/****************************************************************/
/* LISTP(3Q),NTYPEX(4Q),TYPEP(5Q),DTEST(6Q) ,INSTANCEP(303Q)    */
/****************************************************************/
/*
				changed : Jan. 13 1987 take
				changed : Feb. 05 1987 take
				changed : Jul. 24 1987 take

*/

#include "lispemul.h"
#include "lsptypes.h"
#include "cell.h"
#include "lispmap.h"
#include "lspglob.h"


/************************************************************************/
/*									*/
/*			N _ O P _ d t e s t				*/
/*									*/
/*	Check for type conformity, else error.				*/
/*									*/
/************************************************************************/

LispPTR N_OP_dtest(tos, atom_index)
  register LispPTR tos;
  register int atom_index;
  {
    register struct dtd *dtd68k ;

    for(dtd68k=(struct dtd *) GetDTD(GetTypeNumber(tos));
	atom_index != dtd68k->dtd_namelo+(dtd68k->dtd_namehi<<16) ;
	dtd68k=(struct dtd *) GetDTD(dtd68k->dtd_supertype))
      {
	if( dtd68k->dtd_supertype == 0) ERROR_EXIT(tos);
      }
    return(tos);
  } /* OP_DTEST END */





/************************************************************************/
/*									*/
/*			N _ O P _ i n s t a n c e p			*/
/*									*/
/*	Returns T if tos has type named by atom_index, else NIL.	*/
/*									*/
/************************************************************************/

LispPTR N_OP_instancep(tos, atom_index)
  register LispPTR tos;
  register int atom_index;
  {
    register unsigned int type ;
    register struct dtd *dtd68k ;

    for(  dtd68k=(struct dtd *)GetDTD(GetTypeNumber(tos));
	  atom_index != dtd68k->dtd_namelo +(dtd68k->dtd_namehi<<16) ;
	  dtd68k=(struct dtd *)GetDTD(dtd68k->dtd_supertype) )
      {
	if( dtd68k->dtd_supertype== 0) { return(NIL_PTR); }
      }
    return(ATOM_T);

  } /* N_OP_instancep END */

