/* This is G-file @(#) gcscan.c Version 2.5 (4/21/92). copyright Xerox & Fuji Xerox  */
static char *id = "@(#) gcscan.c	2.5 4/21/92";

/*************************************************************************/
/*************************************************************************/
/*	Copyright (C) 1987 by Fuji Xerox Co., Ltd. All rights reserved	 */
/*************************************************************************/
/*                                                                       */
/*                         File Name : gcscans.c                         */
/*                                                                       */
/*************************************************************************/
/*                                                                       */
/*                         Creation Date : July-7-1987                   */
/*                         Written by Tomoru Teruuchi                    */
/*                                                                       */
/*************************************************************************/
/*                                                                       */
/*           Functions :                                                 */
/*                        gcscan1(probe)                                 */
/*                        gcscan2(probe)                                 */
/*                                                                       */
/*************************************************************************/
/*           Descreption :                                               */
/*                                                                       */
/* The functions "gcscan1" and "gcscan2" are the translated functions    */
/*  from the Lisp Functions "\GCSCAN1" & "\GCSCAN2".                     */
/* These functions' role is to scan the HTmain Table and return the      */
/*  existing entry(by "gcscan1") & the entry whose STKBIT field is ON    */
/*  (by "gcscan2").These functions are the UFN functions that are called */
/*  by OPCODES "GCSCAN1" & "GCSCAN2".                                    */
/*                                                                       */
/* gcscan1                                                               */
/*    INPUT : probe (the startng offset in the HTmain table)             */
/*    OUTPUT : the entry's offset or NIL (no more entry existing)        */
/*                                                                       */
/* gcscan2                                                               */
/*    INPUT : probe (the starting offset in the HTmain table)            */
/*    OUTPUT : the entry's offset or NIL (no more entry existing)        */
/*************************************************************************/
/*                                                               \Tomtom */
/*************************************************************************/

#include "lispemul.h"
#include "lspglob.h"
#include "gc.h"
#include "lsptypes.h"

#define HTSTKBIT 		0x200		/* = 512 */
#define HTENDS 			((struct hashentry *) htlptr)
#define GetStkCnt(entry1)	(entry1 >> 9)

/* extern DLword *HTmain;  -- this is real method */


DLword gcscan1 (probe)
register int 	probe;				/* probe is offset */
  { register struct htlinkptr *htlptr;		/* overlay access method */ 
    register int contents;
    while (--probe >= 0)			/* End of HTmain Table ? */
	{ 
					/* Start addr. of scanning */	
   	  htlptr = (struct htlinkptr *)(HTmain+probe);
	  contents = ((struct htlinkptr *)WORDPTR(htlptr))->contents;
	  if (contents && (((struct hashentry *)WORDPTR(HTENDS))->collision || (GetStkCnt(contents) == 0)))
		 return(probe);
	}
    return(NIL);
   }

DLword gcscan2 (probe)
register int 	probe;				/* probe is offset */
   {register struct htlinkptr *htlptr;		/* overlay access method */ 
	while (--probe >= 0)			/* End of HTmain Table ? */
	 { htlptr = (struct htlinkptr *)(HTmain+probe);
						/* Start addr. of scanning */
	   if (((HTSTKBIT | 1) & ((struct htlinkptr *)WORDPTR(htlptr))->contents) != 0)
			return(probe);		/* stackref or collision ON */
	 }
	return(NIL);
   }
