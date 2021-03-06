/* @(#) gcoflow.c Version 1.2 (4/22/92). copyright venue & Fuji Xerox  */
static char *id = "@(#) gcoflow.c	1.2 4/22/92	(venue & Fuji Xerox)";



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
/*************************************************************************/
/*************************************************************************/
/*************************************************************************/
/*                                                                       */
/*                       File Name : gcpunt.c                       */
/*                                                                       */
/*************************************************************************/
/*                                                                       */
/*                      Creation Date : July-8-1987                      */
/*                      Written by Tomoru Teruuchi                       */
/*                                                                       */
/*************************************************************************/
/*                                                                       */
/*           Functions : gc_handleoverflow(arg);                         */
/*                       gcmaptable(arg);                                */
/*                                                                       */
/*************************************************************************/
/*           Descreption :                                               */
/*                                                                       */
/*************************************************************************/
/*                                                               \Tomtom */
/*************************************************************************/

#include "lispemul.h"
#include "lsptypes.h"
#include "address.h"
#include "adr68k.h"
#include "lspglob.h"
#include "gc.h"



#define MAXSMALLP		65535
#define HTBIGENTRYSIZE		4
#define WORDSPERPAGE		256
#define MAXTYPENUMBER		INIT_TYPENUM
#define GetSegnuminColl(entry1) ((entry1 & 0x01fe) >> 1)
#define GetCountinColl(entry1)  ((entry1 & 0x0fc00) >> 10)
#define GetStkCnt(entry1)		 ((entry1 & 0x0fe00) >> 9)
#define Oddp(num) (((num % 2) != 0)?1:0)
#define Evenp(num,prim) (((num % prim) == 0)?1:0)
#define Increment_Allocation_Count(n)			\
    if (*Reclaim_cnt_word != NIL) 			\
		if (*Reclaim_cnt_word > n)		\
			(*Reclaim_cnt_word) -= n;	\
		else { *Reclaim_cnt_word = NIL; 	\
			  doreclaim();  		\
			};				\


gc_handleoverflow(arg)
DLword	arg;
   { struct htoverflow  *cell;
     struct dtd	        *ptr;
     DLword		cellcnt;
     LispPTR            addr;
	  cell = (struct htoverflow *)HToverflow;
			/* This proc. protected from interrupt */
	while((addr = cell->ptr) != NIL)
	   {
		REC_GCLOOKUP(addr, cell->pcase);
		cell->ptr = 0;
		cell->pcase = 0;
		++cell; /* (\ADDBASE CELL WORDSPERCELL) */
	   };
	ptr = (struct dtd *)GetDTD(TYPE_LISTP);
		/* same as "extern struct dtd *ListpDTD" */
	if ((cellcnt = ptr->dtd_cnt0) > 1024)
	   { Increment_Allocation_Count(cellcnt);
		ptr->dtd_oldcnt += cellcnt;
		ptr->dtd_cnt0 = 0;
	   };
	return(arg);
   }

gcmaptable(arg)
DLword	arg;
   { struct htoverflow	*cell;
	struct dtd	*ptr;
	DLword		cellcnt;
	int		typnum;
	LispPTR		addr;			
	cell = (struct htoverflow *)HToverflow;	
			/* This proc. protected from interrupt */
	while((addr = cell->ptr) != NIL)
	   {
		REC_GCLOOKUP(addr, cell->pcase);
		cell->ptr = 0;
		cell->pcase = 0;
		++cell; /* (\ADDBASE CELL WORDSPERCELL) */
	   };
	for(typnum = 1; typnum <= *MaxTypeNumber_word; ++typnum)
	   /* applied alltype */
	   { ptr = (struct dtd *)GetDTD(typnum);
		if ((cellcnt = ptr->dtd_cnt0) != 0)
		   { ptr->dtd_oldcnt += cellcnt;
		     ptr->dtd_cnt0 = 0;
		     Increment_Allocation_Count(cellcnt);
		   };
	   };
	return(arg);
   }




