/* @(#) z2.c Version 2.11 (1/9/89). copyright Venue & Fuji Xerox  */
static char *id = "@(#) z2.c    2.11 1/9/89";
/*
 *      Author :  don charnley
 */
/***********************************************************************/
/*
                File Name :     z2.c

                Including :     N_OP_clfmemb   -- op 035
                                N_OP_classoc   -- op 033
                                N_OP_restlist  -- op 043

*/
/**********************************************************************/


/************************************************************************/
/*                                                                      */
/*      Copyright 1989, 1990 Venue, Fuji Xerox Co., Ltd, Xerox Corp.    */
/*                                                                      */
/*      This file is work-product resulting from the Xerox/Venue        */
/*      Agreement dated 18-August-1989 for support of Medley.           */
/*                                                                      */
/************************************************************************/


#include <stdio.h>
#include "lispemul.h"
#include "emlglob.h"
#include "lspglob.h"
#include "lispmap.h"
#include "lsptypes.h"
#include "address.h"
#include "adr68k.h"
#include "cell.h"
#include "stack.h"
#include "gc.h"
#include "my.h"

struct cadr_cell cadr();

/*   N_OP_classoc()  OP 33Q  */
N_OP_classoc(key, list)
LispPTR key, list;
{
/* register*/ struct cadr_cell cadr1;
register LispPTR        cdr;    /* address of (cdr A-list); Lisp address */


  switch (key & 0xFF0000) {
    case S_POSITIVE:   break;
    case S_NEGATIVE:   break;
    case S_CHARACTER:  break;
    case ATOMS_OFFSET: break;
    default: ERROR_EXIT(list);
  }
  /* JRB - Don Charnley claims the code below should be identical to the
        code in IL:ASSOC, so I copied my new and exciting version over */

        if (list == NIL_PTR) { return(NIL_PTR); }

        if (GetTypeNumber( list ) != TYPE_LISTP) { return(NIL_PTR); }


        S_N_CHECKANDCADR(list, cadr1, list);

        do{
                cdr = cadr1.cdr_cell;   /* the rest of A-list */
                if(Listp(cadr1.car_cell) && key == car(cadr1.car_cell)){
                /* cons data found */
                        return(cadr1.car_cell);
                }
                /* search the rest of A-list */
                if(Listp(cdr))
                        cadr1 = cadr(cdr);
                else cdr = NIL;
                /* check for interrupts and punt to handle one safely */
                if (!Irq_Stk_End) {
                        TopOfStack = cdr; /* for continuation */
                        TIMER_EXIT(cdr);
                }
        }while(cdr != NIL_PTR);

        return(NIL_PTR);
} /* end N_OP_classoc() */

/*   (CL:FMEMB item list)  OP 35Q  */
N_OP_clfmemb(item, list)
register LispPTR item, list;
{  /* OP 35Q */


  switch (item & 0xFF0000) {
    case S_POSITIVE:   break;
    case S_NEGATIVE:   break;
    case S_CHARACTER:  break;
    case ATOMS_OFFSET: break;
    default: ERROR_EXIT(list);
  }
  /* JRB - Don Charnley claims the code below should be identical to IL:FMEMB,
        so I copied it */

        while(Listp(list)) {
                if(item == car(list))
                        return list;
                list = cdr(list);
                /* if we get an interrupt, punt so we can handle it safely */
                if(!Irq_Stk_End) {
                        TopOfStack = list; /* for continuation */
                        TIMER_EXIT(list);
                }
        }
        if(list) ERROR_EXIT( list);
        return list;

} /* end N_OP_clfmemb() */


/************************************************************
         43      RESTLIST

        alpha = skip  --  number of args to skip
        tos = last  --  last arg#
        tos-1 = tail

        IF tail = NIL THEN
                page _ NEXTCONSPAGE
                GOTO make
        ELSE
                AddRef tail
                page _ CONSPAGE[tail]
                GOTO make
        make:
                get [cnt,,next] from page
        make1:
                tail _ CONSCELL (CAR = IVar(last), CDR = tail)
                AddRef IVar(last)
                IF skip = last THEN GOTO fin
                last _ last - 1
                GOTO make1
        noroomonconspage:
        fin:
                store updated [cnt,,next]
                update ListpDTD:COUNTER
                DelRef tail
                IF noroomonconspage THEN UFN
                ELSEIF ListpDTD:COUNTER overflow then GCPUNT
                ELSEIF overflow entries then GCHANDLEOVERFLOW
                ELSE NEXTOPCODE

        alpha = skip  --  number of args to skip
        tos = last  --  last arg#
        tos-1 = tail

        AddRef tail
make1:  tail , cons(IVar(last), tail)
        AddRef IVar(last)
        IF skip = last THEN GOTO fin
        last _ last - 1
        GOTO make1

fin:    DelRef tail

***********************************************************/

N_OP_restlist(tail, last, skip)
register LispPTR tail;
register int last, skip;
{

  last &= 0xFFFF;
  while (skip <= last) {
    tail = cons(GetLongWord(IVar + (--last << 1)), tail);
  }
  return(tail);
}/* end N_OP_restlist() */

     /* end module */
