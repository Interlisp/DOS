/* This is G-file @(#) return.c Version 2.8 (4/21/92). copyright Xerox & Fuji Xerox  */
static char *id = "@(#) return.c	2.8 4/21/92";
/*
 *	Copyright (C) 1987 by Fuji Xerox Co., Ltd. All rights reserved.
 *
 *	Author :  Takeshi Shimizu
 *
 */
/***********************************************************/
/*
		File Name :	return.c
		Including	:	OP_contextsw
					contextsw

		Created	:	May 1, 1987 Takeshi Shimizu
		Changed :	May 19 1987 take
				Aug 27 1987 NMitani
				Sep.02 1987 take
				Sep.09 1987 take
				Oct.23 1987 Take


*/
/***********************************************************/
#include <stdio.h>

#include "lispemul.h"
#include "address.h"
#include "adr68k.h"
#include "lsptypes.h"
#include "lispmap.h"
#include "stack.h"
#include "emlglob.h"
#include "lspglob.h"
#include "initatms.h"
#include "cell.h"
#include "return.h"

/***********************************************************************/
/*
		Func Name :	OP_contextsw
		Created	:	Jul 3, 1987 Takeshi Shimizu
		changed 	AUG 25 1987 TAKE
				aug 31 take
				Aug 4  1987 NMitani
				Oct 23 1987 Take
				Nov 05 1987 Take(modify flags,del. whocalls
					and incall args)

		Desc	:	Execute ContextSW to FX specified as 
				offset from IFPGE by TOS .
*/
/***********************************************************************/


OP_contextsw()
{
#ifdef TRACE
	printf("OP_contextsw:\n");
#endif
  contextsw(TopOfStack & 0xffff,1,2);
  /* TOS will be smashed ?? I'm not sure .
	PC will be incremented 1. */

} /* OP_contextsw */


contextsw(fxnum,bytenum,flags)
 register DLword fxnum;
 register DLword bytenum; /* BYTEnum that you want increment PC 
				after CONTEXTSW */
 register DLword flags;  /* 0bit(MSB) ON: incall mode */
			 /* 1bit ON : call from OP_contextsw */
			 /* I don't know that it is the possible case that 
				flags is 3 . */ 
  {
    register DLword *next68k;
    register DLword *freeptr ; /* point to STK to be FSB */

#ifdef TRACE 
    printf("contextsw : %d \n",fxnum);
#endif

    if(!(fxnum==SubovFXP))
      {
	/* interrupt disable during execting [special] function
	  invoked by contextsw(\KEYHANDLER,\RESETSTACK,FAULT)
	 */
      }

    if(flags & 1) /* INCALL? */
	error("contextswitch sets Incall");
    else
	CURRENTFX->nopush = T;


  /* store PC */
    CURRENTFX->pc = ((int)PC - (int)FuncObj) + bytenum;

 /* TOS save */
    if(flags & 2)
      {
  	PushStack(fxnum);
 	CurrentStackPTR += 2;
      }
    else
      {
	PushCStack;
 	CurrentStackPTR += 2;
      }
  CURRENTFX->nextblock=LOLOC(LADDR_from_68k(CurrentStackPTR));
  
  /* FSB set */
  GETWORD(CurrentStackPTR)=STK_FSB_WORD;
  GETWORD(CurrentStackPTR+1)= (((int)EndSTKP-(int)CurrentStackPTR)>>1);

#ifdef STACKCHECK
	if(EndSTKP < CurrentStackPTR) error("contextsw:Illegal ESP");
#endif


  Midpunt(fxnum); /* exchanging FX */

RTNX :
  next68k = (DLword *)Addr68k_from_LADDR(STK_OFFSET | CURRENTFX->nextblock);

  if(GETWORD(next68k) != STK_FSB_WORD) error("contextsw(): MP9316");
   freeptr=next68k;

/* Merging FSB area */
  while(GETWORD(freeptr) == STK_FSB_WORD)  EndSTKP=freeptr=freeptr+  GETWORD(freeptr+1);



#ifdef DEBUG
  printf("contextsw:ESTKP set ");
  laddr(EndSTKP);
#endif

	if(CURRENTFX->incall)
	 {    error("return to frame with incall bit ");
	 }
	else 	 
	 {
		if (CURRENTFX->nopush)
		 {
#ifdef DEBUG
			printf("context:after:nopush \n");
#endif

			CURRENTFX->nopush =NIL;
			CurrentStackPTR = next68k-2 ;
			TopOfStack = *((LispPTR *)CurrentStackPTR) ;
			CurrentStackPTR-=2;

		 }
		else
		 {
#ifdef DEBUG
			printf("context:after:3 \n");
#endif
		 	CurrentStackPTR = next68k-2 /*-1*/ ; /* CHanged by Hayata */
		 }

#ifdef STACKCHECK
		CHECKFX;
		if( EndSTKP< CurrentStackPTR) 
			error("contextsw:Illegal ESP");
#endif
		FastRetCALL;

		return;
	 }

} /* end contextsw */

