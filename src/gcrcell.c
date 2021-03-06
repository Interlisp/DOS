/* @(#) gcrcell.c Version 1.2 (4/22/92). copyright venue & Fuji Xerox  */
static char *id = "@(#) gcrcell.c	1.2 4/22/92	(venue & Fuji Xerox)";





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
/*                                                                       */
/*                       File Name : gcrcell.c                    	 */
/*                                                                       */
/*************************************************************************/
/*                                                                       */
/*                      Creation Date : July-7-1987                      */
/*                      Written by Tomoru Teruuchi                       */
/*                                                                       */
/*************************************************************************/
/*                                                                       */
/*           Functions :                                                 */
/*                       gcreccell(cell);				 */
/*                       freelistcell(cell);                             */
/*                                                                       */
/*                                                                       */
/*************************************************************************/
/*           Descreption :                                               */
/*                                                                       */
/*  The functions "gcreccell" and "freelistcell" are the translated 	 */
/*  functions from the Lisp functions "\GCRECLAIMCELL" that is the UFN   */
/*  function of the opcode "RECLAIMCELL", and "\FREELISTCELL".           */
/*  These functions may have the following characteristics :             */
/*                                                                       */
/*  	gcreccell(cell)	LispPTR cell					 */
/* 		This function may always return NIL(= 0), as the Lisp    */
/*           	macro .RECLAIMCELLLP. in more upper level may use this   */
/*              return value as the further Garbage's pointer.(The Opcode*/
/*              "RECLAIMCELL"'s function is specified as this, but its   */
/*              UFN function is not. The gcreccell function's		 */
/*              behavior is same as the UFN function for speed and 	 */
/*		simplicity,this is, this function is closed in this level*/
/*		)							 */
/*		This function may reclaim the data of all types that is  */
/*		Garbage.Especially, the data whose types are ARRAYBLOCK  */
/*		(= 0), STACKP(= 8),VMEMPAGEP(= 10) and CODEBLOCK(= 54,55,*/
/*		56,57,58,59,60,61,62,63) may be reclaimed by each special*/
/*		processes that are specified and invoked by this function*/
/*		.The data whose type is LISTP is the main data type	 */
/*		processed in this function actually and only then the 	 */
/*		function "freelistcell" may be called for making linkage */
/*		of free list.						 */
/*									 */
/*	freelistcell(cell)	LispPTR cell				 */
/*		This function may make the linkage of free list of the	 */
/*		cons cell.The header of this linkage is DTD->NEXTPAGE of */
/*		LISTP and each cons page has its internal linkage of free*/
/*		cells.This return value is not considered as not used.	 */
/*									 */
/*************************************************************************/
/*                                                               \Tomtom */
/*************************************************************************/

#include <stdio.h>
#include "lispemul.h"
#include "lsptypes.h"
#include "address.h"
#include "adr68k.h"
#include "lspglob.h"
#include "stack.h"
#include "cell.h"
#include "ifpage.h"
#include "gc.h"
#include "dbprint.h"

#define CONSPAGE_LAST	0x0ffff


/************************************************************************/
/*									*/
/*				g c r e c c e l l			*/
/*									*/
/*	Reclaim a cell, doing necessary finalization &c.		*/
/*									*/
/************************************************************************/

gcreccell(cell)
  LispPTR	cell;
  {
    register ConsCell *ptr;
    struct dtd *typdtd; 
    DLword typ; 
    register LispPTR tmpptr, donext, tmpcell, val;
    LispPTR ptrfield, carfield; 
    int index, code;
    LispPTR *field;
    int freecnt;
    LispPTR freeptr;
    LispPTR *actptr;

    val = NIL;
    tmpptr = cell;
    index = -1;
    donext = NIL;
  lp:
    ptr = (ConsCell *)Addr68k_from_LADDR(tmpptr);
/* # ifdef CHECK
  if (refcnt(tmpptr) != 1) error("reclaiming cell w/refcnt not 1");
 # endif
*/
#ifdef DEBUG
    if (tmpptr & 1) error("Reclaiming cell pointer with low bit 1.");
#endif

    typ = GetTypeNumber(tmpptr);
#ifdef DEBUG
    if (typ==6) printf("Reclaiming array ptr 0x%x.\n", tmpptr);
#endif
    switch(typ) 
      { 
	 case TYPE_LISTP: 
		{ 
		  if ((code = ptr->cdr_code) == CDR_INDIRECT) /* indirect */
		    { tmpcell = ptr->car_field;  /* Monitor */
		      freelistcell(tmpptr);
		      ptr = (ConsCell *) Addr68k_from_LADDR(tmpcell);
		      tmpptr = tmpcell;
		      code = ptr->cdr_code;
		    };
		  if (index != -1)	/* car part */
		    index = -1;
/*		  else if ((val = htfind(car(tmpptr), DELREF)) != NIL) */
		  else
		    {
		      REC_GCLOOKUPV(car(tmpptr), DELREF, val);
		      if (val != NIL)
			{ 
			  ptr->car_field = donext;
			  ptr->cdr_code = code;
			  donext = tmpptr; 
			  goto doval;
			};
		    }
/*		  val = htfind(cdr(tmpptr),DELREF); */
		  REC_GCLOOKUPV(cdr(tmpptr), DELREF, val);
		  if (code <= CDR_MAXINDIRECT)
		    {
		      tmpcell = POINTER_PAGEBASE(tmpptr)
				    + ((code - CDR_INDIRECT) << 1);
		      freelistcell(tmpcell);
		    };
		  freelistcell(tmpptr);
		  goto doval;    
		};
	case TYPE_ARRAYBLOCK:
		if ((index == -1) && reclaimarrayblock(tmpptr)) 
		  goto trynext;
		else break;	
	case TYPE_STACKP:
		if ((index == -1) && reclaimstackp(tmpptr)) goto trynext; 
		break;	
	case TYPE_VMEMPAGEP:
		if ((index == -1) && releasingvmempage(tmpptr))
		  {
		    goto trynext;
		  }
		else break;
	case TYPE_CODEHUNK1:
	case TYPE_CODEHUNK2:
	case TYPE_CODEHUNK3:
	case TYPE_CODEHUNK4:
	case TYPE_CODEHUNK5:
	case TYPE_CODEHUNK6:
	case TYPE_CODEHUNK7:
	case TYPE_CODEHUNK8:
	case TYPE_CODEHUNK9:
	case TYPE_CODEHUNK10:
		if ((index == -1) && reclaimcodeblock(tmpptr)) goto trynext;
		else break; 
	default:  ;
      }; 
  normal: 
    typdtd = (struct dtd *)GetDTD(typ); 
    ptrfield = typdtd->dtd_ptrs; 
    if (index != -1)
      {
	index = (index << 1);
	ptrfield = cdr(ptrfield);
	while ((car(ptrfield) & 0x0ffff) != index) ptrfield = cdr(ptrfield);
	index = -1;
      };	
    while(ptrfield != NIL)
      {
	carfield = car(ptrfield);
	ptrfield = cdr(ptrfield);
	carfield &= 0x0ffff;
/*
	val = GCLOOKUPV(0x8000,DELREF,(0x0ffffff & *(LispPTR *)
		Addr68k_from_LADDR(tmpptr+carfield)));
*/
	REC_GCLOOKUPV((0x0ffffff &
		        *(LispPTR *)Addr68k_from_LADDR(tmpptr+carfield)),
		       DELREF, val);

	if (val != NIL)
	  {
	    if (ptrfield != NIL)
	      {
		ptr = (ConsCell *)Addr68k_from_LADDR(tmpptr);
		ptr->car_field = donext;
		ptr->cdr_code = ((car(ptrfield) & 0x0ffff) >> 1);
		donext = tmpptr;
		goto doval;
	      }
	    else goto addtofreelist;
	  };
      };
  addtofreelist:
    field = (LispPTR *)Addr68k_from_LADDR(tmpptr);
    *field = typdtd->dtd_free;
    typdtd->dtd_free = tmpptr;

  doval:
    if (val != NIL)
      {
	tmpptr = val;
	REC_GCLOOKUP(tmpptr, ADDREF);
/*	GCLOOKUP(0x8000, ADDREF,tmpptr); */
	val = NIL;
	goto lp;
      };
  trynext:
    if (donext != NIL)
      {
	tmpptr = donext;
	ptr = (ConsCell *)Addr68k_from_LADDR(tmpptr);
	donext = (LispPTR)ptr->car_field;
	index = ptr->cdr_code;
	goto lp;
      };
    return(NIL);
  }


/************************************************************************/
/*									*/
/*			f r e e l i s t c e l l				*/
/*									*/
/*									*/
/*									*/
/************************************************************************/

freelistcell(cell)
LispPTR	cell;
  {
    struct conspage *pbase; 
    register ConsCell *cell68k;
    cell68k = (ConsCell *)Addr68k_from_LADDR(cell);
    pbase = (struct conspage *)Addr68k_from_LPAGE(POINTER_PAGE(cell));
    cell68k->cdr_code = pbase->next_cell;
    pbase->next_cell = (cell & 0x0ff);
    if ((++pbase->count > 2) && (pbase->next_page == CONSPAGE_LAST)) 
      {
	pbase->next_page = ListpDTD->dtd_nextpage;  
	ListpDTD->dtd_nextpage = POINTER_PAGE(cell); 
      };
		 	   
  }

