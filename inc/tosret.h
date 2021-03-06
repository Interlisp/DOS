/* @(#) tosret.h Version 1.1 (4/21/92). copyright Venue & Fuji Xerox  */

/************************************************************************/
/*									*/
/*	Copyright 1989, 1990 Venue, Fuji Xerox Co., Ltd, Xerox Corp.	*/
/*									*/
/*	This file is work-product resulting from the Xerox/Venue	*/
/*	Agreement dated 18-August-1989 for support of Medley.		*/
/*									*/
/************************************************************************/




#ifdef NATIVETRAN
#define RETD6 SaveD6 = 0x100
#define RET_CHECK_NATIVE(x) if(x ->native) { RET_TO_NATIVE; }
#else
#define RETD6
#define RET_CHECK_NATIVE(x)
#endif


#if (defined(SUN3_OS3_OR_OS4_IL) &&  !(defined(NOASMFNCALL)) )

#define OPRETURN							\
{	opreturn();							\
	EXT; if(slowreturn()) goto stackoverflow_help; RET;		\
	Irq_Stk_Check = STK_END_COMPUTE(EndSTKP,FuncObj);		\
	if (((int)(CSTKPTR) > Irq_Stk_Check) || (Irq_Stk_End <= 0))	\
			{ RETD6; goto check_interrupt;	}		\
	Irq_Stk_End = (int) EndSTKP;					\
	RET_CHECK_NATIVE(BCE_CURRENTFX);				\
 }

#else

#define OPRETURN	{						\
 register struct frameex2 *returnFX ;					\
 register int alink;							\
 alink = ((struct frameex2 *) BCE_CURRENTFX)->alink;			\
 FNTPRINT(("RETURN.\n"));						\
 if (alink & 1) { EXT; if(slowreturn()) goto stackoverflow_help; RET;	\
	Irq_Stk_Check = STK_END_COMPUTE(EndSTKP,FuncObj);		\
	if (((int)(CSTKPTR) > Irq_Stk_Check) || (Irq_Stk_End <= 0))	\
			{ RETD6; goto check_interrupt;	}		\
	Irq_Stk_End = (int) EndSTKP;					\
	RET_CHECK_NATIVE(BCE_CURRENTFX);				\
	goto retxit;							\
	};								\
 CSTKPTRL = (LispPTR *) IVAR;						\
 returnFX = (struct  frameex2 *)					\
	((DLword *)							\
	    (PVARL = (DLword *) Addr68k_from_StkOffset(alink))		\
	    - FRAMESIZE);						\
 IVARL = (DLword *)							\
	    Addr68k_from_StkOffset(GETWORD((DLword *)returnFX -1));	\
	/* Get PC from Retunee's pc slot in FX */			\
 PCMACL = returnFX->pc  + (ByteCode *)					\
	(FuncObj = (struct fnhead *)					\
	Addr68k_from_LADDR(SWAP_WORDS(returnFX->fnheader) & 0x0ffffff)) + 1;\
 Irq_Stk_Check = STK_END_COMPUTE(EndSTKP,FuncObj);			\
  FNCHECKER(if (quick_stack_check()) printf("In RETURN.\n"));	\
 if (((int)(CSTKPTR) > Irq_Stk_Check) || (Irq_Stk_End <= 0))		\
		{ RETD6; goto check_interrupt;	}		\
 Irq_Stk_End = (int) EndSTKP;						\
 RET_CHECK_NATIVE(returnFX);						\
retxit:	 {}								\
} /* OPRETURN end */

#endif
