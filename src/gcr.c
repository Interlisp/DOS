/* @(#) gcr.c Version 1.2 (4/22/92). copyright venue & Fuji Xerox  */
static char *id = "@(#) gcr.c	1.2 4/22/92	(venue & Fuji Xerox)";



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
/*                       File Name : gcr.c                         */
/*                                                                       */
/*************************************************************************/
/*                                                                       */
/*                      Creation Date : Oct-12-1987                      */
/*                      Written by Tomoru Teruuchi                       */
/*                                                                       */
/*************************************************************************/
/*           Functions : gcarrangementstack()                            */
/*			 doreclaim();                                    */
/*                       dogc01();                                       */
/*                       disablegc1(noerror);                            */
/*                                                                       */
/*************************************************************************/
/*           Description :						 */
/* This files' functions is the invocator that may invoke the reclaimer. */
/*  gcarrangementstack()						 */
/*	This function's role is  narrowing the gap between the		 */
/*      contextswitch and the subrcall.					 */
/*	In the original Lisp Source, as the contextswitch process may    */
/*	clear the remain area to FSB,there is no problem in scannig stack*/
/*	.But in the subrcall,there isn't such process.			 */
/*	Therefore, the function is required to set the remain stack area */
/*	 to FSB. And this function does so.				 */
/*  dogc01()								 */
/*	This function is the mere caller of the reclaimer.		 */
/*	The callees are gcscanstack(), gcmapscan() and gcunmapscan().    */
/*  doreclaim()								 */
/*	This function is the real top level function of the reclaimer.   */
/*	But currently this function is not used(may be used in Future.)  */
/*	This function may have a problem. It is to manipulate "clock"    */
/*	for keeping the GC's time period.				 */
/*   disablegc1(noerror)
/*      This function is the rescue function,when the HTcoll table is    */
/*      overflow and so on.After this function's process is over, the    */
/*      keyhandler will sense the interrupt table state and call the     */
/*      function \DOGCDISABLEDINTERRUPT for reporting to Users that      */
/*      this system status is dangerous and you should save your works.  */
/*                                                                       */
/*************************************************************************/
/*                                                               \tomtom */
/*************************************************************************/

#include "lispemul.h"
#include "lispmap.h"
#include "emlglob.h"
#include "lsptypes.h"
#include "address.h"
#include "adr68k.h"
#include "lspglob.h"
#include "stack.h"
#include "gc.h"



#define MAXSMALLP		65535
#define HTBIGENTRYSIZE		4
#define WORDSPERPAGE		256
#define WORDSPERCELL		2
#define MAXTYPENUMBER		INIT_TYPENUM
#define STK_HI			1

#ifndef BYTESWAP
   struct interruptstate
 {      unsigned nil1             :3;
        unsigned gcdisabled       :1;
	unsigned vmemfull         :1;
	unsigned stackoverflow    :1;
	unsigned storagefull      :1;
	unsigned waitinginterrupt :1;
	unsigned nil2             :8;
	DLword   intcharcode;
	};
#else
   struct interruptstate
 {
	DLword   intcharcode;
	unsigned nil2             :8;
	unsigned waitinginterrupt :1;
	unsigned storagefull      :1;
	unsigned stackoverflow    :1;
	unsigned vmemfull         :1;
        unsigned gcdisabled       :1;
	unsigned nil1             :3;
	};
#endif /* BYTESWAP */


gcarrangementstack()
{  LispPTR tmpnextblock;
	PushCStack;
	tmpnextblock = LADDR_from_68k(CurrentStackPTR+=WORDSPERCELL);
	CURRENTFX->nextblock = LOLOC(tmpnextblock);
  GETWORD(CurrentStackPTR)=STK_FSB_WORD;
  GETWORD(CurrentStackPTR+1)=(((int)EndSTKP-(int)CurrentStackPTR)>>1);
}


/****************************************************************/
/* The following function is the caller that is the reclaimer.  */
/* And, this function is same as \DOGC1 in Lisp because in the  */
/* C's implimentation the contextswitch is not required for the */
/* remaining the system status.					*/
/****************************************************************/

dogc01()
{
 gcarrangementstack();
 gcscanstack();
 gcmapscan();
 gcmapunscan();
 PopCStack;
 return(NIL);
}

/*!!!!!! should update clock in Miscstats */

doreclaim()
{int gctm1;
 MISCSTATS gcmisc;

   if (*GcDisabled_word == NIL)
	{update_miscstats();
	 gcmisc = *((MISCSTATS *)MiscStats);
	 *Reclaim_cnt_word = NIL;
	 if (*GcMess_word != NIL)
		flip_cursor();
	 dogc01();
	 if (*GcMess_word != NIL)
		flip_cursor();
	 *Reclaim_cnt_word = *ReclaimMin_word;
	 update_miscstats();
	 MiscStats->gctime = MiscStats->gctime +
			MiscStats->totaltime - gcmisc.totaltime +
			MiscStats->swapwaittime - gcmisc.swapwaittime;
	};
}


disablegc1(noerror)
int       noerror;
{ struct interruptstate *gcinterruptstate;
  int                   count, i;
  DLword                typeword;
  gcinterruptstate = (struct interruptstate *)
			Addr68k_from_LADDR(*INTERRUPTSTATE_word);
  count = (128)*256; /* This is test value. 128 is *MdsTTsize(\MDSTTsize) */
  for(i = 0;i < count;i++)
    {typeword = GETWORD((DLword *)Addr68k_from_LADDR(LADDR_from_68k(MDStypetbl)+i));
     GETWORD((DLword *)Addr68k_from_LADDR(LADDR_from_68k(MDStypetbl)+i) )
                 = (typeword | TT_NOREF);
   };
  *Reclaim_cnt_word = NIL;
  *ReclaimMin_word  = NIL;
  if ((noerror == NIL) && (*GcDisabled_word == NIL))
    { gcinterruptstate->gcdisabled = T;
      *PENDINGINTERRUPT_word = ATOM_T;
    };
  *GcDisabled_word = ATOM_T;
  return(NIL);
}
