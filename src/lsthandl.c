/* @(#) lsthandl.c Version 1.2 (4/22/92). copyright venue & Fuji Xerox  */
static char *id = "@(#) lsthandl.c      1.2 4/22/92     (venue & Fuji Xerox)";

/*
 *
 *      Author :  Takeshi Shimizu
 *
 */
/***********************************************************************/


/************************************************************************/
/*                                                                      */
/*      (C) Copyright 1989-92 Venue. All Rights Reserved.               */
/*      Manufactured in the United States of America.                   */
/*                                                                      */
/*      The contents of this file are proprietary information           */
/*      belonging to Venue, and are provided to you under license.      */
/*      They may not be further distributed or disclosed to third       */
/*      parties without the specific permission of Venue.               */
/*                                                                      */
/************************************************************************/
/*
                File Name :     lsthandl.c

                                Date :          Jul. 4, 1987
                                Edited by :     Takeshi Shimizu
                                Changed :       16-Oct,1987 take

                Including :     OP_fmemb
                                OP_listget


*/
/**********************************************************************/

#include "lispemul.h"
#include "emlglob.h"
#include "lspglob.h"
#include "lsptypes.h"
#include "address.h"
#include "adr68k.h"
#include "cell.h"


/***********************************************************************/
/*       N_OP_fmemb                                                     */
/**********************************************************************/



LispPTR N_OP_fmemb(item, tos)
        register LispPTR item, tos;
{  /* OP 34Q */

        while(Listp(tos)) {
                if(item == car(tos))
                        return tos;
                tos = cdr(tos);
                /* if we get an interrupt, punt so we can handle it safely */
                if(!Irq_Stk_End) {
                        TIMER_EXIT(tos);
                }
        }
        if(tos) ERROR_EXIT(tos);
        return tos;

} /* N_OP_fmemb end */

/***********************************************************************/
/*       
        Func Name :     fmemb(item,list)
        >>For User programming<<
        NOTE: You should not handle long list, because it doesn't care
                about interrupt.

*/
/**********************************************************************/

LispPTR fmemb(item, list)
        register LispPTR item, list;
{  

        while(Listp(list)) {
                if(item == car(list))
                        return (list);
                list = cdr(list);
        }
        if(list) return(list);
        return (list);

} /* fmemb end */

/***********************************************************************/
/*
                Func Name :     N_OP_listget
                Opcode  :       47Q
 */
/**********************************************************************/

extern struct cadr_cell cadr(); /** declaration only **/

#define SAVE_ERROR_EXIT2(topcstk,tos) {Scratch_CSTK=topcstk; ERROR_EXIT(tos);}

#define S_N_CHECKANDCADR2(sour,dest,tos,tcstk)             \
                { register LispPTR parm = sour;                  \
                 if(GetTypeNumber(parm) != TYPE_LISTP){         \
                        SAVE_ERROR_EXIT2(tcstk,tos);       \
                }else                                           \
                        dest = cadr(parm);                      \
                }

LispPTR N_OP_listget(plist, tos)
        register LispPTR plist;
         register LispPTR tos;
{
 /* register*/ struct cadr_cell cadrobj;

 while ( plist != NIL_PTR ) {
        S_N_CHECKANDCADR2(plist, cadrobj, tos, plist);

        if ( cadrobj.car_cell == tos ) {
                if(cadrobj.cdr_cell == NIL_PTR) return NIL_PTR;
                if(Listp(cadrobj.cdr_cell))
                        return(car(cadrobj.cdr_cell));
                else    /* must punt in case car/cdrerr */
                        SAVE_ERROR_EXIT2(plist,tos);
                }

        if( ! Listp(cadrobj.cdr_cell) )
                { /* this list ended before we find prop */
                return ( NIL_PTR );
                }

        S_N_CHECKANDCADR2(cadrobj.cdr_cell, cadrobj, tos, plist);
        plist = cadrobj.cdr_cell;

        if (!Irq_Stk_End) {
                /* for continuation, it becomes plist on next time */
                Scratch_CSTK = plist;
                TIMER_EXIT(tos);
                }
        }

 return(NIL_PTR);

}/* N_OP_listget end */
