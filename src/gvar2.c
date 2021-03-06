/* @(#) gvar2.c Version 2.10 (4/21/92). copyright venue & Fuji Xerox  */
static char *id = "@(#) gvar2.c	2.10 4/21/92		(venue & Fuji Xerox)";




/************************************************************************/
/*									*/
/*	Copyright 1989, 1990 Venue.					*/
/*									*/
/*	This file was work-product resulting from the Xerox/Venue	*/
/*	Agreement dated 18-August-1989 for support of Medley.		*/
/*									*/
/************************************************************************/



#include <stdio.h>
#include "lispemul.h"
#include "lsptypes.h"
#include "lspglob.h"
#include "adr68k.h"
#include "gc.h"
#include "emlglob.h"
#include "cell.h"
#include "dbprint.h"



/************************************************************************/
/*									*/
/*			    N _ O P _ g v a r _				*/
/*									*/
/*	GVAR_ opcode (027).  Assign a value to a global variable.	*/
/*									*/
/*	atom_index is the "atom number," either the lo half of the	*/
/*	old litatom, or the new-atom itself.				*/
/*									*/
/*	* call gclookup with DELREF and address of GVAR slot.		*/
/*	* call gclookup with ADDREF and TopOFStack.			*/
/*	* replace GVAR slot with tos.					*/
/*	* If Hash Table is overflow, call fn1ext.			*/
/*									*/
/************************************************************************/
 
LispPTR N_OP_gvar_(tos, atom_index)
  register LispPTR tos;
  unsigned int atom_index;
  {
    register LispPTR *pslot;	/* pointer to argued GVAR slot */

#ifdef BIGATOMS
    if (0 != (atom_index & 0xFF0000))
      pslot = (LispPTR *) Addr68k_from_LADDR(atom_index + NEWATOM_VALUE_OFFSET);
    else
#endif /* BIGATOMS */

    pslot = (LispPTR *) Valspace + atom_index;
    DEBUGGER(if (tos&0xFF000000) error("Setting GVAR with high bits on"));
    FRPLPTR(((struct xpointer*)pslot)->addr, tos);
    return(tos);
  }




/************************************************************************/
/*									*/
/*			    N _ O P _ r p l p t r			*/
/*									*/
/*	RPLPTR opcode (024).  Replace a pointer field somewhere,	*/
/*	updating the reference counts for the old value and the new	*/
/*	value (DELREF and ADDREF, respectively).			*/
/*									*/
/*	tos_m_1 is the base, and alpha is a word-offset for finding	*/
/*	the cell to replace contents of.				*/
/*	tos is the new value.						*/
/*									*/
/************************************************************************/

LispPTR N_OP_rplptr(tos_m_1, tos, alpha)
  register LispPTR tos, tos_m_1;
  unsigned int alpha;
  {
    register DLword *pslot;	/* pointer to argued slot (68 address) */

    pslot = Addr68k_from_LADDR(tos_m_1 + alpha );
    FRPLPTR( ((struct xpointer*)pslot)->addr,tos);
    return(tos_m_1);
  }


