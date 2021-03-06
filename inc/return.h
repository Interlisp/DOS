/* This is G-file @(#) return.h Version 2.3 (3/23/89). copyright Venue & Fuji Xerox  */
/**************************************************************/
/*

	File Name : 	return.h
	Desc. :		Macros for return,contextsw

	Written by :	Takeshi Shimizu
			11-May-88

*/
/**************************************************************/


/************************************************************************/
/*									*/
/*	Copyright 1989, 1990 Venue, Fuji Xerox Co., Ltd, Xerox Corp.	*/
/*									*/
/*	This file is work-product resulting from the Xerox/Venue	*/
/*	Agreement dated 18-August-1989 for support of Medley.		*/
/*									*/
/************************************************************************/


/* FAST case return use */

#define FastRetCALL							\
  {									\
    /* Get IVar from Retunee's IVAR offset slot(BF) */ 			\
    IVar = Addr68k_from_LADDR(STK_OFFSET | GETWORD((DLword *)CURRENTFX -1)); \
    /* Get FuncObj from Retunee's FNHEAD slot in FX */ 			\
    FuncObj = (struct fnhead *)						\
		Addr68k_from_LADDR((CURRENTFX->hi2fnheader <<16)	\
					| CURRENTFX->lofnheader);	\
    /* Get PC from Retunee's pc slot in FX */ 				\
    PC = (ByteCode *)FuncObj + CURRENTFX->pc ; 				\
  }



/** in CONTEXTSW , for exchanging context **/

#define Midpunt(fxnum) 							\
  { register DLword midpunt; 					\
    midpunt = LOLOC(LADDR_from_68k(CURRENTFX));			\
    PVar=(DLword *)							\
	    Addr68k_from_StkOffset(					\
			(GETWORD(((DLword *)InterfacePage) +fxnum)))	\
		+ FRAMESIZE; 						\
    GETWORD(((DLword *)InterfacePage) +fxnum) = midpunt ;		\
  }


#define CHECKFX							\
  if (((int)PVar -(int)CURRENTFX) != 20)			\
    { printf("Invalid FX(0x%x) and PV(0x%x) \n",		\
	     LADDR_from_68k(CURRENTFX),LADDR_from_68k(PVar));	\
    }



/**** Calls when invoke the function is assumed
	that it is called by CONTEXTSW in orginal LISP code **/

#define BEFORE_CONTEXTSW						\
  { CurrentStackPTR += 2; 						\
    CURRENTFX->nextblock=StkOffset_from_68K(CurrentStackPTR); 		\
    GETWORD(CurrentStackPTR)=STK_FSB_WORD; 				\
    GETWORD(CurrentStackPTR+1)= (((int)EndSTKP-(int)(CurrentStackPTR))>>1); \
  }


#define AFTER_CONTEXTSW							\
  { register DLword *ptr68k,*freeptr;					\
    ptr68k = (DLword*)Addr68k_from_StkOffset(CURRENTFX->nextblock);	\
    if(GETWORD(ptr68k) != STK_FSB_WORD) error("pre_moveframe: MP9316");	\
    CHECK_FX(CURRENTFX);						\
    freeptr=ptr68k;							\
    while(GETWORD(freeptr) == STK_FSB_WORD)  				\
      EndSTKP=freeptr=freeptr+  GETWORD(freeptr+1);			\
    S_CHECK(CURRENTFX->incall== NIL, "CONTEXTSW during fn call");	\
    S_CHECK(CURRENTFX->nopush== NIL, "CONTEXTSW, NOPUSH is set");	\
    CurrentStackPTR = ptr68k- 2 ;					\
    CHECK_FX(CURRENTFX);						\
    S_CHECK( EndSTKP > CurrentStackPTR, 				\
		"End of stack isn't beyond current stk pointer."); 	\
  }

