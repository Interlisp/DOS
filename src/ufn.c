/* This is G-file @(#) ufn.c Version 2.6 (4/21/92). copyright Venue & Fuji Xerox  */
static char *id = "@(#) ufn.c	2.6 4/21/92";
/*
 *	Copyright (C) 1987 by Fuji Xerox Co., Ltd. All rights reserved.
 *
 *	Author :  Takeshi Shimizu

 *
 */
/******************************************************************/
/*

		File Name  :	ufn.c(for use with SYSOUT)
		Including  :	ufn

		Created    :	jun 8, 1987 by T.Shimizu


*/
/******************************************************************/


/************************************************************************/
/*									*/
/*	Copyright 1989, 1990 Venue, Fuji Xerox Co., Ltd, Xerox Corp.	*/
/*									*/
/*	This file is work-product resulting from the Xerox/Venue	*/
/*	Agreement dated 18-August-1989 for support of Medley.		*/
/*									*/
/************************************************************************/


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

#ifdef C_ONLY

#define GetUFNEntry(num)	(((UFN *)UFNTable) + (num))


/******************************************************************/
/*

		Func Name  :	ufn
		Desc  :		call Lisp UFN func
				TopOfStack;
				PC ;

		Created    :	May 29, 1987 by T.Shimizu
		Changed	   :	Jun 17 1987 take
				Aug 25 1987 take
				Sep  4  1987 NMitani
				Oct 07 1987 take
				Nov 13 1987 take +8 & Simple STKCHK
				Nov 25 1987 take(tmp_fn)
*/
/******************************************************************/

ufn(bytecode)
DLword bytecode ;
{
  register DefCell *defcell68k ;  /* Definition Cell PTR */
  register int pv_num ; /* scratch for pv */
  register UFN *entry68k;
  register struct fnhead *tmp_fn;
  register unsigned int arg_num ;    /* Num of args */
  register int rest ;  /* use for arignments */


#ifdef TRACE
	printPC();
	print_atomname("TRACE : ufn() 0%o ",bytecode);
#endif

  PushCStack;

  entry68k = (UFN *)GetUFNEntry(bytecode); /*changing I/F 17-jun*/

#ifdef TRACE
	if(entry68k->atom_name)
	  printf("Atom number: %d\n", 0xffff && entry68k->atom_name);
	else
	  error("UNF not specified");
#endif

  switch(entry68k->byte_num)
   {
	case 0 :	break;
	case 1 :	/*PushStack(SPOS_HI  | Get_BYTE(PC+1));*/
			CurrentStackPTR +=2;
			GETWORD(CurrentStackPTR)=SPOS_HI;
			GETWORD(CurrentStackPTR+1)=(DLword)Get_BYTE(PC+1);
	/* I think we don't have to shift alpha byte eight bit before save it. */
#ifdef DEBUG
		printf("***ufn: case 1\n");
#endif
		break;

	case 2 :	/*PushStack(S_POSITIVE |Get_DLword(PC+1));*/
			CurrentStackPTR +=2;
			GETWORD(CurrentStackPTR) = SPOS_HI ;
			GETWORD(CurrentStackPTR+1) = (DLword)((Get_BYTE(PC+1) << 8) | Get_BYTE(PC+2));

#ifdef DEBUG
		printf("***ufn: case 2\n");
#endif
		break;
	default : error("ufn : Bad UFN MP 9351 ");
		break;
   }

  /* Get Next Block offset form OPCODE byte */
  CURRENTFX->nextblock = (LADDR_from_68k(CurrentStackPTR) & 0x0ffff)
					- (entry68k->arg_num<<1) +2 /** +1 **/ ;

  /* Setup IVar */
  IVar = Addr68k_from_LADDR((((LispPTR)(CURRENTFX->nextblock)) | STK_OFFSET)) ;

#ifdef LISPTRACE
	print(entry68k->atom_name);printf(": ");
	{int cnt;
		for(cnt=0;cnt<arg_num;cnt++){
			printf(" IVAR%d :",cnt);
			print(*((LispPTR *)(IVar + (cnt*2))));
		}
	}
	printf("\n");
#endif
  /* Set PC to the Next Instruction and save into FX */
  CURRENTFX->pc = ((unsigned int)PC - (unsigned int)FuncObj) + entry68k->byte_num +1;  



  defcell68k =(DefCell *) GetDEFCELL68k(entry68k->atom_name) ;

  if( defcell68k->ccodep == 0 )
   {	   /* This LispFunc is NOT compiled object . We must use Interpreter*/
	printf("UFN: UFN func isn't compiled OBJ \n");
	defcell68k = (DefCell *)GetDEFCELL68k(ATOM_INTERPRETER);
	PushStack(TopOfStack);  /* Move AtomIndex to CSTK */

   }


   /* Nov 25 87 take */
  tmp_fn = (struct fnhead *)Addr68k_from_LADDR( defcell68k->defpointer ) ;

/* stack overflow check STK_SAFE is redundant?? */
 if ( (unsigned int)(CurrentStackPTR + tmp_fn->stkmin +STK_SAFE) >=  (unsigned int)StkLimO ) 
 {
	/**printf("#### STKOVER in UFN case\n");**/

	DOSTACKOVERFLOW(entry68k->arg_num,entry68k->byte_num); /* After STKOVR, retry current OPCODE */

 }

  FuncObj =tmp_fn ;

 if( FuncObj->na >= 0 )
  {
	/* This Function is Spread Type */
	/* Arguments on Stack Adjustment  */
	rest = entry68k->arg_num - FuncObj->na ;
	
	while(rest <0) { 
	  PushStack(NIL_PTR) ;
	  rest++;
	}
	CurrentStackPTR -= (rest<<1) ;
  } /* if end */

 /* Set up BF */
 CurrentStackPTR +=2;
 GETWORD(CurrentStackPTR) = BF_MARK ;
 GETWORD(CurrentStackPTR+1) = CURRENTFX->nextblock ;
 CurrentStackPTR +=2;
/*  *(++CurrentStackPTR) = CURRENTFX->nextblock ; */

 /* Set up FX */
 GETWORD(CurrentStackPTR) = FX_MARK;


 ((struct frameex1 *)CurrentStackPTR)->alink = LADDR_from_68k(PVar);
 PVar = (DLword *)CurrentStackPTR + FRAMESIZE ;
 ((struct frameex1 *)CurrentStackPTR)->lofnheader = 
	(defcell68k->defpointer) & 0x0ffff ;
 ((struct frameex1 *)CurrentStackPTR)->hi2fnheader = 
	((defcell68k->defpointer) & 0x0ff0000) >> 16 ;

 CurrentStackPTR = PVar ;

 /* Set up PVar area */
 pv_num = FuncObj->pv +1 ; 

 while(pv_num > 0) {
	*((LispPTR *)CurrentStackPTR) = 0x0ffff0000 ;
	CurrentStackPTR += DLWORDSPER_CELL ;
	*((LispPTR *)CurrentStackPTR) = 0x0ffff0000 ;
	CurrentStackPTR += DLWORDSPER_CELL ; 
	pv_num--;
  }

/*  CurrentStackPTR ++ ;*/

 /* Set PC points New Function's first OPCODE */
 PC =(ByteCode *) ((unsigned int)FuncObj + FuncObj->startpc) ;

/** TopOfStack = (FuncObj->startpc -1) >> 1;**/

} /* end ufn */


#endif

