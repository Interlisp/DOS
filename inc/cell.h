/* @(#) cell.h Version 2.15 (4/21/92). copyright Venue & Fuji Xerox  */

/*
 *	Author	:	Takeshi Shimizu
 *			Hiroshi Hayata
 */





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


/**********************************************************************/
/*
		File Name :	cell.h

		Cell Manipulate Macros

				Date :		December 16, 1986
				Edited by :	Takeshi Shimizu

*/
/**********************************************************************/

/*  CONS CELL (LISTP) definitions moved to lispemulater.h */

  /* This Macro may produce the CDR code */
#define  CDRCODE(x)     LRSH (((int)x & 0x0ff000000) , 24)

  /* This Macro may produce a pointer that points CAR cell */
  /* On 68010,68000 This Macro does not effect *
#define  CARFIELD(x)    ((int)x & 0x00ffffff)

/* CDR-Codes defs */
#define CDR_ONPAGE		128
#define CDR_NIL			128
#define CDR_INDIRECT		0
#define CDR_MAXINDIRECT	127

extern short  CarCdrError ;

#ifndef BYTESWAP

struct
  conspage
    {
      unsigned    count : 8   ;
      unsigned    next_cell :8 ;
      DLword  next_page ;
    };

#else
/* byte-swapped version */
struct
  conspage
    {
      DLword  next_page ;
      unsigned    next_cell :8 ;
      unsigned    count : 8   ;
    };

#endif /* BYTESWAP */


/* Following MACROs for Conspage */
/* lisp_ptr is LISP pointer, returns 68k ptr points struct conspage obj */
#define Get_ConsPageBase(lisp_ptr)	(struct conspage *)Addr68k_from_LPAGE(POINTER_PAGEBASE(lisp_ptr))
#define GetNewCell_68k(conspage68k)	 (ConsCell *)(((DLword *)(conspage68k)) + (unsigned)((conspage68k)->next_cell) )

/* page : LISP page */
#define GetCONSCount(page)	(((struct conspage *)Addr68k_from_LPAGE(page))->count)

#ifndef BYTESWAP
	/************************************************/
	/*  Definitions for normal byte-order machines	*/
	/************************************************/
/* DEFs for DEFINITIONCELL */
typedef
  struct definition_cell
    {
      unsigned	ccodep	:	1 ;
      unsigned	fastp	:	1 ;
      unsigned	argtype	:	2 ;
      unsigned	pseudocodep :	1 ;
      unsigned	byteswapped :	1 ;	/* T for native-order fn opcodes */
      unsigned	nil	:	2 ;
      unsigned	defpointer :	24;

    } DefCell ;

typedef
  struct pname_cell
    {
      unsigned	pkg_index : 8 ;
      unsigned	pnamebase : 24 ;
    } PNCell ;

typedef
  struct proplist_cell
    {
      unsigned unused: 1;
      unsigned gensymp: 1;
      unsigned fatpnamep: 1;
      unsigned nil: 5;
      unsigned propbase: 24;
    } PLCell;


struct xpointer{
	unsigned	flags:8;
	unsigned	addr:24;
};


#else
	/************************************************/
	/*  Definitions for byte-swapped machines	*/
	/************************************************/
/* DEFs for DEFINITIONCELL */
typedef
  struct definition_cell
    {
      unsigned	defpointer :	24;
      unsigned	nil	:	2 ;
      unsigned	byteswapped : 	1 ;	/* T if opcodes are native-order */
      unsigned	pseudocodep :	1 ;
      unsigned	argtype	:	2 ;
      unsigned	fastp	:	1 ;
      unsigned	ccodep	:	1 ;

    } DefCell ;

typedef
  struct pname_cell
    {
      unsigned	pnamebase : 24 ;
      unsigned	pkg_index : 8 ;
    } PNCell ;

typedef
  struct proplist_cell
    {
       unsigned propbase: 24;
      unsigned nil: 5;
      unsigned fatpnamep: 1;
      unsigned gensymp: 1;
     unsigned unused: 1;
    } PLCell;

struct xpointer
  {
    unsigned	addr:24;
    unsigned	flags:8;
  };


#endif /* BYTESWAP */




struct
  cadr_cell
    {
      LispPTR car_cell;	/* Lisp address (word addressing) */
      LispPTR cdr_cell;	/* Lisp address (word addressing) */
    };



/************************************************************************/
/*									*/
/*	Access to the parts of a SYMBOL: Pname, Definition, Value, 	*/
/*	and property list.						*/
/*									*/
/*									*/
/*									*/
/*									*/
/************************************************************************/

#ifndef BIGATOMS
#define GetDEFCELL68k(index)	((LispPTR *)Defspace + (index) )
#define GetVALCELL68k(index)	((LispPTR *)Valspace + (index))
#define GetPnameCell(index)	((LispPTR *)Pnamespace + (index))
#define GetPropCell(index)	((LispPTR *)Plistspace + (index))

  /* Good only for old-style LITATOMS */
#define GetDEFCELLlitatom(index)	((LispPTR *)Defspace + (index) )
#define GetVALCELLlitatom(index)	((LispPTR *)Valspace + (index))
#define GetPnameCelllitatom(index)	((LispPTR *)Pnamespace + (index))
#define GetPropCelllitatom(index)	((LispPTR *)Plistspace + (index))

#else
  /* Good for old LITATOMS and new NEW-ATOMs */
#define GetDEFCELL68k(index)	(((index & 0xFF0000) != 0)	?	   \
				  (LispPTR *)(Addr68k_from_LADDR(index)+NEWATOM_DEFN_OFFSET) \
				: ((LispPTR *)Defspace + (index) ))

#define GetVALCELL68k(index)	(((index & 0xFF0000) != 0)	?	    \
				  (LispPTR *)(Addr68k_from_LADDR(index)+NEWATOM_VALUE_OFFSET) \
				:  ((LispPTR *)Valspace + (index)))

#define GetPnameCell(index)	(((index & 0xFF0000) != 0)	?	   \
				  (LispPTR *)(Addr68k_from_LADDR(index)+NEWATOM_PNAME_OFFSET) \
				:  ((LispPTR *)Pnamespace + (index)))

#define GetPropCell(index)	(((index & 0xFF0000) != 0)	?	   \
				  (LispPTR *)(Addr68k_from_LADDR(index)+NEWATOM_PLIST_OFFSET) \
				:  ((LispPTR *)Plistspace + (index)))

  /* Good only for old-style LITATOMS */
#define GetDEFCELLlitatom(index)	((LispPTR *)Defspace + (index) )
#define GetVALCELLlitatom(index)	((LispPTR *)Valspace + (index))
#define GetPnameCelllitatom(index)	((LispPTR *)Pnamespace + (index))
#define GetPropCelllitatom(index)	((LispPTR *)Plistspace + (index))

  /* Good only for new-style NEW-ATOMs */
#define GetDEFCELLnew(index) (LispPTR *)(Addr68k_from_LADDR(index)+NEWATOM_DEFN_OFFSET)
#define GetVALCELLnew(index) (LispPTR *)(Addr68k_from_LADDR(index)+NEWATOM_VALUE_OFFSET)
#define GetPnameCellnew(index) (LispPTR *)(Addr68k_from_LADDR(index)+NEWATOM_PNAME_OFFSET)
#define GetPropCellnew(index) (LispPTR *)(Addr68k_from_LADDR(index)+NEWATOM_PLIST_OFFSET)

#endif /* BIGATOMS */




/* When cadr() function is called, type check should be done. */

#define	S_N_CHECKANDCADR(sour, dest, tos)			\
		{register LispPTR parm = sour;			\
		 if(GetTypeNumber(parm) != TYPE_LISTP){		\
			ERROR_EXIT(tos);			\
		}else						\
			dest = cadr(parm);			\
		}


