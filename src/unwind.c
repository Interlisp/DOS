/* @(#) unwind.c Version 2.22 (4/21/92). copyright Venue & Fuji Xerox  */
static char *id = "@(#) unwind.c	2.22 4/21/92		(Venue & Fuji Xerox)";


/************************************************************************/
/*									*/
/*	Copyright 1989, 1990 Venue, Fuji Xerox Co., Ltd, Xerox Corp.	*/
/*									*/
/*	This file is work-product resulting from the Xerox/Venue	*/
/*	Agreement dated 18-August-1989 for support of Medley.		*/
/*									*/
/************************************************************************/


/******************************************************************/
/*

		File Name  :	unwind.c

		Created    :	jul 17, 1987 by T.Shimizu
		Changed	   :	Sept 21 1988  BK

*/
/******************************************************************/

#include "lispemul.h"
#include "emlglob.h"
#include "stack.h"
#include "lspglob.h"


LispPTR N_OP_unwind(cstkptr, tos, n, keep)
	register LispPTR *cstkptr;
	register LispPTR tos;
	int n;
	int keep;

{ register int  num ;  /* number of UNBOUND slot */
  register LispPTR *endptr; /* unwind limit */
  register LispPTR *lastpvar; /* points PVar slot that is unbounded. */


/* Slots:
	-----------------
	|		|	<- PVar
	-----------------
	|	.	|
	|	.	|
	-----------------
	|		|	 ALL OF THE FOLLOWING LOCATIONS SCANNED:
	-----------------------------------------------------------------
	| tos if keep	|	<- endptr (PVar[n]) <- Result (no keep)	|
	-----------------						|
	|		|	<- Result (keep)			|
	-----------------						|
	|		|						|
	-----------------						|
	|	.	|						|
	|	.	|						|
	-----------------						|
	|   tos pushed	|	<- Start CSTKPTR			|
	-----------------------------------------------------------------
	|		|	<- CSTKPTR temporarily bumped pushing tos
	-----------------

	NOTE: upon return the emulator does a POP to get the new tos value

*/


endptr = (LispPTR *) PVar + n;			/* set unwind limit */

if (endptr > cstkptr) {
	CurrentStackPTR = (DLword *) cstkptr;
	ERROR_EXIT(tos);
	}
*cstkptr++ = tos;

	/* UNBOUND MARK loop  */

while (cstkptr > endptr)
   {
		/* Look for the Next BIND marker */

	if( (num = (int) *--cstkptr) < 0 )
	   {

		/* Now UNBIND the PVARS indicated by the BIND marker */

	    lastpvar = (LispPTR *) (2 + PVar + (unsigned short) num) ;
	    num = ~(num >> 16) + 1;
	    for(; --num > 0; )
	     { *--lastpvar = 0xffffffff ; /* Mark as UNBOUND */ }
	    };
   };


/* endptr = cstkptr */

 if(keep)
  {
	*(cstkptr++) = tos;
  }
 return((LispPTR) cstkptr);

} /* N_OP_unwind */

#ifdef CATCH

/******************************************************************/
/*
	SUBR: FIND_THE_BLIP

	find the blip down in the stack.  Similar to the LISP
	SI::UNWIND-TO-BLIP, except the target is returned instead
	of doing the unwinding.  Note that the unwider parameter
	must already be properly set by the caller (i.e. to the
	alink if necessary).
*/
/******************************************************************/
#define SMALLP(x) (((unsigned int)x >> 16) == (S_POSITIVE >> 16))

find_the_blip(blip, throwp, unwinder)
	register LispPTR blip;
	register LispPTR throwp;
	FX *unwinder;

{	register LispPTR target;
	register FX *target_addr;
	register LispPTR pc = NIL_PTR;

LispPTR CATCH_RETURN_PC_ATOM = parse_atomstring("SI::*CATCH-RETURN-PC*");
LispPTR CHATCH_RETURN_TO_ATOM = parse_atomstring("SI::*CATCH-RETURN-TO*");
LispPTR CATCH_RETURN_FROM_ATOM = parse_atomstring("SI::*CATCH-RETURN-FROM*");

	for (target = (LispPTR) unwinder;
			FX_INVALIDP(target);
			GETCLINK(target_addr)) {
	    target_addr = (FX *) Addr68k_from_StkOffset(target);
	    if (blip == *target_addr)	{
		register LispPTR var_name_in_frame =
		    variable_name_in_frame(target_addr, (FVPVAR << 8) + 1);
		if (var_name_in_frame == CHATCH_RETURN_TO_ATOM) {
		    if (throwp) {
			pc = pvar_value_in_frame(target_addr, CATCH_RETURN_PC_ATOM);
			if !(SMALLP(pc))
			    error("Catch return-to frame lacks PC");
			}
		    goto cons_result;
		} else
		if (var_name_in_frame == CATCH_RETURN_FROM_ATOM) {
		    if (throwp) {target = GETCLINK(target_addr); };
		    goto cons_result;
		}
		} /* if blip */
	    }; /* for */


no_result:
	return(NIL_PTR);
cons_result:
	return(cons(StkOffset_from_68K(target), pc));
}


LispPTR variable_name_in_frame(fx_addr, code)
	FX *fx_addr;
	register LispPTR code;
{
	register DLword *name_ptr;
	register DLword *name_bind_ptr;
	register DLword *name_table_base;
	register DLword value;

	name_table_base = Addr68k_from_LADDR(GETNAMETABLE(fx_addr));
	name_ptr = name_table_base + FNHEADSIZE;
	name_table_base = name_ptr + ((FNHEAD *) name_table_base)->ntsize;

	while (value = GETWORD(name_ptr++)) do {
	    if (code == GETWORD(name_bind_ptr++)) {return(value) } ;
	    }; /* while */
	return(NIL_PTR);
}; /* variable_name_in_frame */

/******************************************************************/
/*
	pvar_value_in_frame searches through the name table for
	frame_addr looking for the binding of atom_index.  It
	returns the PVAR value of the binding if the PVAR is found
	& it is bound.
*/
/******************************************************************/

LispPTR pvar_value_in_frame(frame_addr, atom_index)
	register  FX		*frame_addr;
	register  LispPTR	atom_index;

{
	register DLword *name_ptr;
	register DLword *name_bind_ptr;
	register DLword *name_table_base;
	register DLword value;

	name_table_base = Addr68k_from_LADDR(GETNAMETABLE(frame_addr));
	name_ptr = name_table_base + FNHEADSIZE;
	name_table_base = name_ptr + ((FNHEAD *) name_table_base)->ntsize;

	while (value = GETWORD(name_ptr++)) do {
	    if (value == atom_index) {
		register DLword bind_info = GETWORD(name_bind_ptr);
		if ((bind_info >> 8) == FVPVAR) {
		    register LispPTR slot_value =
			*((LispPTR *) (FRAMESIZE + (DLword *) frame_addr) +
				(bind_info - (FVPVAR << 8)))
		    if (slot_value > 0) { return(slot_value & 0xffffff); };
		    };
		};
	    name_bind_ptr++;
	    }; /* while */
	return(NIL_PTR);

}; /* pvar_value_in_frame */


#endif
