/* @(#) llstk.c Version 1.25 (4/21/92). copyright venue & Fuji Xerox  */
static char *id = "@(#) llstk.c	1.25 4/21/92	(venue & Fuji Xerox)";


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
        File Name :     llstk.c
        Desc.     :     Low Level stack operations
        Including :

        Edited by :     Takeshi Shimizu(March 14, 1988)

*/
/******************************************************************/
#include "lispemul.h"
#include "lispmap.h"
#include "adr68k.h"
#include "address.h"
#include "lsptypes.h"
#include "initatms.h"
#include "lspglob.h"
#include "emlglob.h"
#include "cell.h"
#include "stack.h"
#include "return.h"

extern int extended_frame;

/******************************************************************/
/*
        Func Name :     do_stackoverflow(incallp)

		retval: If There is no space for stack then return 1
					else return 0
		incallp:
			If Calling during function call,incallp=T
			else NIL
        Edited by :     Take(March 28, 1988)
*/
/******************************************************************/
do_stackoverflow(incallp)
  int incallp;
  {
    register DLword *next68k;
    DLword newfx;
    DLword savenext;
    DLword *oldPVar;
    int movedistance;
    LispPTR moveframe();
#ifdef STACKCHECK
    LispPTR stackcontents;
    LispPTR TopIVAR;

    stackcontents= *((LispPTR*)CurrentStackPTR);
    TopIVAR= *((LispPTR*)IVar);
#endif

   /* Don't care PC,FuncObj, */
   /*if incall flag ON ,Don't care IVar
	,became resudual and it is pointed by copied FX's BLINK*/
    oldPVar=PVar;

    if(*NeedHardreturnCleanup_word)
      {
	warn("HardreturnCleanup in do_stackoverflow");
      }
    if(incallp)
      {
	savenext=CURRENTFX->nextblock; /* save old nextblock */
      }

    BEFORE_CONTEXTSW;	/* Don't Use MIDPUNT and Don't care IFPAGE */


    /* Call MOVEFRAME directly */
    if((newfx=(DLword)moveframe(CURRENTFX)) == 0xFFFF)
      {
	 /* To make immidiately call HARDRESET */
	Irq_Stk_Check = 0;
	Irq_Stk_End=0;
	return(1); /* Whole space exausted */
      }


    /* Return from MOVEFRAME directory */

    PVar=(DLword*)Addr68k_from_StkOffset(newfx+FRAMESIZE);
    movedistance= ((int)PVar-(int)oldPVar) >>1;
    AFTER_CONTEXTSW;

    if(incallp)
      {
	/* set next(it pointed to old IVar) with offset */
	CURRENTFX->nextblock= savenext + movedistance;

	/* including Last Arg(kept in TOS */
	S_CHECK(FuncObj == (struct fnhead *)
			Addr68k_from_LADDR(
				(CURRENTFX->hi2fnheader << 16) |
				CURRENTFX->lofnheader),
		 "in call, but stack frame doesn't match FN being executed." );
	CHECK_FX(CURRENTFX);

	 /* We should re-Set up IVAR,CURRENTFX->nextblock */
	 IVar += movedistance;
      } /* incallp */

    return(0); /* Normal return */
    /* If  incallp ,we CAN continue executing FN or APPLY by just returning */
    /* new PVar will set in funcall */
  }/* end do_stackoverflow */




/******************************************************************/
/*
        Func Name :     moveframe(oldfx68k)

        Edited by :     Take(March 14, 1988)
*/
/******************************************************************/
LispPTR moveframe(oldfx68k)
  register FX *oldfx68k;
  {
    register int size;
    register DLword *next68k;
    register DLword *new68k;
    DLword *freestackblock();
    DLword *extendstack();
    int nametbl_on_stk =NIL;
    int at_eos = NIL;

    PreMoveFrameCheck(oldfx68k);
#ifdef FLIPCURSOR
    flip_cursorbar(10);
#endif

    size = FX_size(oldfx68k) + DLWORDSPER_CELL;
    S_CHECK(size > 0, "size of stack block < 0");
    next68k=Addr68k_from_StkOffset(oldfx68k->nextblock);

tryfsb:
    if(FSBP(next68k))
      {
        /* merge free blocks */
        new68k = next68k + FSB_size(next68k);
	if(FSBP(new68k))
	  {
	    for(;FSBP(new68k); new68k = new68k + FSB_size(new68k))
                FSB_size(next68k) += FSB_size(new68k);
	    new68k=(DLword *)oldfx68k;
	    goto out;
	  }
	else if(StkOffset_from_68K(new68k) == InterfacePage->endofstack)
          {
	    if((StkOffset_from_68K(new68k) > LOLOC(*GuardStackAddr_word)) &&
		((*STACKOVERFLOW_word)== NIL) )
	      at_eos = T; /* search FSB in earler STACK area by freestackblock */
            else if(extendstack() != NIL)
	      {
		new68k=(DLword *)oldfx68k;
		goto out;
	      }
	    else
	      {
		/* These lines are different from Original Code */
		return(0xFFFF); /* No space */
	      }
          }/* else if end */

      }

    CHECK_FX(oldfx68k);

    S_CHECK(oldfx68k->usecount == 0, "use count > 0");
    /* we don't check \INTERRUPTABLE */

    if(oldfx68k->validnametable && (oldfx68k->hi2nametable==STK_HI))
      {
	/* frame contains a name table, so we care that the alignment
	 of the new block be same as old */
#ifdef STACKCHECK
        { DLword n;
	  n=oldfx68k->lonametable;
	  if((n <= StkOffset_from_68K(oldfx68k))
                  && (n >= oldfx68k->nextblock))
	    {  WARN("moveframe:check!!", sff(LADDR_from_68k(oldfx68k)));}
        }
#endif
	nametbl_on_stk = T;
	/* Find a free stack block */
	new68k=freestackblock(size,oldfx68k,
               (LADDR_from_68k(oldfx68k)-DLWORDSPER_CELL)% DLWORDSPER_QUAD);
      }
    else
	new68k=freestackblock(size,oldfx68k,-1);/* Not needed to align */

    if(new68k==0) return(0xFFFF);  /* exhausted */

    if( at_eos && ((unsigned int)new68k > (unsigned int)oldfx68k))
      {
	/* extendstack already done in freestackblock */
	((STKBLK*)new68k)->flagword = STK_FSB_WORD;
	goto tryfsb;
      }

    /* copy frame and dummy bf pointer too */
    blt(new68k,(((DLword*)oldfx68k) - DLWORDSPER_CELL),size);

    ((Bframe*)new68k)->residual =T;
    new68k = new68k + DLWORDSPER_CELL; /* now NEW points to the FX */
    ((FX*)new68k)->nextblock = (StkOffset_from_68K(new68k) + size)
				-DLWORDSPER_CELL;
    /* (CHECK (fetch (BF CHECKED) of (fetch (FX BLINK) of OLDFRAME)))*/
    CHECK_BF(Addr68k_from_StkOffset(GETBLINK(oldfx68k)));

    /* Set true BFptr,not residual */
    SETBLINK(new68k,GETBLINK(oldfx68k));

    if(nametbl_on_stk)
      {
	S_CHECK(( (((int)new68k - (int)oldfx68k)>>1) % DLWORDSPER_QUAD) == 0,
		 "Misalignment of stack blocks, with nametable on stack");

	((FX*)new68k)->lonametable += (((int)new68k - (int)oldfx68k)>>1);
      }
    if( ((Bframe*)DUMMYBF(oldfx68k))->residual )
      {
        MAKEFREEBLOCK(((DLword*)oldfx68k)-DLWORDSPER_CELL , size);
      }
    else
      {
        MAKEFREEBLOCK(oldfx68k,size - DLWORDSPER_CELL);
      }

out:
#ifdef FLIPCURSOR
    flip_cursorbar(10);
#endif

    return(S_POSITIVE|StkOffset_from_68K(new68k));
  } /* moveframe end */




/******************************************************************/
/*
        Func Name :     extendstack()
        Desc.     :     if LastStackAddr_word is exceeded,then allocate
                        one new lisppage for STACK area.

        Edited by :     Take(March 14, 1988)

*/
/******************************************************************/
DLword *extendstack()
  {
    register LispPTR easp;
    register LispPTR scanptr;

    easp = InterfacePage->endofstack;

    if (easp < LOLOC(*LastStackAddr_word))
      {
	if((easp > LOLOC(*GuardStackAddr_word)) &&
                ((*STACKOVERFLOW_word)== NIL) )
	  { extended_frame = 1;
            ((INTSTAT*)Addr68k_from_LADDR(*INTERRUPTSTATE_word))->stackoverflow = 1;
             *STACKOVERFLOW_word = *PENDINGINTERRUPT_word=ATOM_T;
          }
        newpage(STK_OFFSET|(scanptr=easp+2));
        /* I don't concern about DOLOCKPAGES */

        MAKEFREEBLOCK(Addr68k_from_StkOffset(scanptr),DLWORDSPER_PAGE-2);
	InterfacePage->endofstack = scanptr=easp+DLWORDSPER_PAGE;
        SETUPGUARDBLOCK(
		Addr68k_from_StkOffset(InterfacePage->endofstack), 2);
        MAKEFREEBLOCK(Addr68k_from_StkOffset(easp) , 2);
        return((DLword*)Addr68k_from_StkOffset(scanptr));
      }
    else return(NIL);
  } /* end extendstack */





/******************************************************************/
/*
        Func Name :     freestackblock(n,sart,align)
        Desc.     :     Search the FSB has specified size n or more
                        Return useful area's ptr.
				   If there is no space for STACK,return 0

        Edited by :     take(15-Jul-87)
                        take(11-Apr-88)
*/
/******************************************************************/

DLword *freestackblock( n , start68k , align )
  DLword n;     /* size you want(in DLword) */
  StackWord *start68k; /* searching will start68k at here */
  int align; /* if Negative,it needn't align */
  {
    register int wantedsize;
    register StackWord *scanptr68k;
    register StackWord *orig68k;
    register STKBLK *freeptr68k;
    register StackWord *easp68k;
    register DLword freesize;

    DLword *extendstack();

    /* compute actually size you needed */
    wantedsize = n + STACKAREA_SIZE + MINEXTRASTACKWORDS;

    easp68k =(StackWord*) (Addr68k_from_StkOffset(InterfacePage->endofstack));

    /*** DEBUG ***/
    S_CHECK(n>2, "asking for block < 2 words long");
    S_CHECK(start68k != 0, "start68k = 0");
    S_CHECK(start68k >=
	    (StackWord*)Addr68k_from_StkOffset(InterfacePage->stackbase),
	    "start68k before stack base");

STARTOVER :
    if(start68k) scanptr68k = start68k;
    else scanptr68k =
	    (StackWord *)Addr68k_from_StkOffset(InterfacePage->stackbase);

SCAN :  switch((unsigned)(STKWORD(scanptr68k)->flags))
	  {
            case STK_FSB :  goto FREESCAN;
                            break;
            case STK_GUARD: if(scanptr68k < easp68k)
                            goto FREESCAN;
                            if (start68k)
			      {
                                scanptr68k =(StackWord *)Addr68k_from_StkOffset
                                		(InterfacePage->stackbase);
                                goto SCAN;
			      }
                            else goto NEWPAGE;
                            break;
            case STK_FX :   scanptr68k =(StackWord *)Addr68k_from_StkOffset(
					    ((FX*)scanptr68k)->nextblock);
                            break;
            default :       orig68k = scanptr68k;

                            while(STKWORD(scanptr68k)->flags != STK_BF)
			      {
                                S_WARN(STKWORD(scanptr68k)->flags==STK_NOTFLG,
					"NOTFLG not on", scanptr68k);
			        scanptr68k = (StackWord *)
					       (((DLword *)scanptr68k)
					            + DLWORDSPER_CELL);
                             };

#ifdef STACKCHECK
			   if(((Bframe*)scanptr68k)->residual)
                             { if(scanptr68k != orig68k)
                                {
				  WARN("freestackblock:scanptr68k !=org",
					printf(":0x%x\n",
						LADDR_from_68k(scanptr68k)));
				}
			     }
			  else
			    {
			      if(((Bframe*)scanptr68k)->ivar
					!= StkOffset_from_68K(orig68k))
				{
				  WARN("BF doesn't point TopIVAR",
					printf(":0x%x\n",
						LADDR_from_68k(scanptr68k)));
				}
			    }
#endif
		/* Used to be a +=, but SunOS4/Sparc compiles it wrong */
                            scanptr68k = (StackWord *)
					    ((DLword *)scanptr68k
						+ DLWORDSPER_CELL);
                            break;

      } /* end switch(scanptr68k */

NEXT: if(scanptr68k != start68k)
      {
	S_CHECK(scanptr68k <=easp68k, "scan ptr past end of stack");
	goto SCAN;
      }
NEWPAGE : easp68k =(StackWord*)extendstack();
	  if(easp68k) goto STARTOVER;
	  else
	   {
	    warn("freestackblock:StackFull MP9319");
	    return(0);
	   }

FREESCAN :      freeptr68k=(STKBLK *)scanptr68k;
                freesize=FSB_size(freeptr68k);
FREE:           scanptr68k = (StackWord *)(((DLword *)freeptr68k) + freesize);

                switch((unsigned)(STKWORD(scanptr68k)->flags))
		  {
		    case STK_FSB:  freesize = freesize + FSB_size(scanptr68k);
                                   goto FREE;
                                    
		    case STK_GUARD: if(scanptr68k<easp68k)
				      {
					freesize = freesize + FSB_size(scanptr68k);
					goto FREE;
                                       }
                                     break;

		    default :       break;

 		  } /* end switch(scanp.. */

    if(freesize >= wantedsize)
      {
	if((align<0) ||
	    (align==(StkOffset_from_68K(freeptr68k) % DLWORDSPER_QUAD)))
          wantedsize= MINEXTRASTACKWORDS;
	else wantedsize= MINEXTRASTACKWORDS+DLWORDSPER_CELL;

	scanptr68k =(StackWord*) (((DLword *)freeptr68k) + wantedsize);

	SETUPGUARDBLOCK(scanptr68k, n);
	MAKEFREEBLOCK(freeptr68k,wantedsize);
	MAKEFREEBLOCK(((DLword*)scanptr68k) + n, freesize - wantedsize -n);
	return((DLword*)scanptr68k);
      }
    else MAKEFREEBLOCK( freeptr68k, freesize);

    goto NEXT;
  } /* freestackblock end */




/******************************************************************/
/*
        Func Name :     decusecount68k(frame)
        Desc.     :     Search the FSB has specified size n or more
                        Return useful are ptr.

        Edited by :     take(March 14, 1988)
*/
/******************************************************************/
#define BF_size(ptr68k) ((StkOffset_from_68K(ptr68k)) - ((Bframe*)(ptr68k))->ivar + 2)

decusecount68k(frame68k)
  register FX *frame68k;
{
 DLword *alink68k;
 register Bframe *blink68k;
 DLword *clink68k;
 register DLword *ivar68k;
 register int size;

if(FX_INVALIDP(frame68k)) return;
CHECK_FX(frame68k);
/* I don't check if \INTERRUPTABLE is NIL */
 while(StkOffset_from_68K(frame68k))
 {
        if(frame68k->usecount != 0)
        {
                frame68k->usecount--;
                return;
         }
        else
        {
                alink68k = Addr68k_from_StkOffset(GETALINK(frame68k));
                blink68k =(Bframe*) Addr68k_from_StkOffset(GETBLINK(frame68k));
                clink68k = Addr68k_from_StkOffset(GETCLINK(frame68k));

                size = FX_size(frame68k);

                if(((Bframe*)DUMMYBF(frame68k))->residual)
                { /* this frame has dummy BF */
                        MAKEFREEBLOCK(((DLword*)frame68k) -DLWORDSPER_CELL,size+DLWORDSPER_CELL);
                }
                else
                {
                        MAKEFREEBLOCK(frame68k,size);
                }

                if(blink68k->usecnt !=0)
                {
                        blink68k->usecnt--;
                }
                else
                {
                    /***    ivar68k=Addr68k_from_StkOffset(blink68k->ivar);
                        GETWORD(ivar68k)=STK_FSB_WORD;
                        GETWORD(ivar68k+1)=ivar68k -(DLword *)blink68k +2; **/
				MAKEFREEBLOCK(Addr68k_from_StkOffset(blink68k->ivar),
							BF_size(blink68k));
                }
                if(alink68k!= clink68k)
                        decusecount68k(alink68k);

                frame68k = (FX*)clink68k;

        } /* else end */

 }/*while end */
} /* decusecount68k end */

#ifdef ORG_FILPCORSORBAR
extern DLword *EmCursorBitMap68K;
extern int LispWindowFd;
extern struct cursor CurrentCursor;
 flip_cursorbar(n)
int n;
{
 GETWORD(EmCursorBitMap68K +n) = ~(GETWORD(EmCursorBitMap68K +n));
#ifdef SUNDISPLAY
 win_setcursor(LispWindowFd,&CurrentCursor);
#endif /* SUNDISPLAY */

#ifdef XWINDOW
 Set_XCursor( 0, 0 );
#endif /* XWINDOW */

}
#else
extern short *DisplayRegion68k;
extern int DisplayRasterWidth;
 flip_cursorbar(n)
int n;
{
register short *word;
word = DisplayRegion68k + (n * DisplayRasterWidth);
GETWORD(word) ^= 0xFFFF;
}
#endif

/**************************************************************/
/*
                blt(dest,source,size)
*/
/**************************************************************/
blt(dest68k,source68k,nw)
 register DLword *source68k;
 register DLword *dest68k;
  int nw;
{
/******* OLD def ,
 Due to C compiler's bug, we can't use pre-decriment for register val
  source68k += nw;
  dest68k += nw;
  while(nw)
  {
    GETWORD(--dest68k)= GETWORD(--source68k);
    nw--;
   }
**** OLD def ****/
  source68k = source68k + nw-1;
  dest68k = dest68k + nw-1;
  while(nw--)
  {
    GETWORD(dest68k--) = GETWORD(source68k--);
  }


}



/**************************************************************/
/*
		stack_check(start68k)
		for DEBUG
*/
/**************************************************************/
stack_check(start68k)
  StackWord *start68k;
  {
    StackWord *scanptr68k;
    StackWord *endstack68k;
    DLword *top_ivar;
    DLword save_nextblock;
    DLword savestack1,savestack2;
    DLword setflg=NIL;
    DLword freesize;

#ifdef FSBCHECK
    struct big_fsbs {   DLword offset; DLword size;} bigFSB[100];
    int bigFSBindex=0;

    bzero((char*)bigFSB, 100 * 2 * 2);
#endif

    if((CURRENTFX->nextblock != StkOffset_from_68K(CurrentStackPTR))
	||(!FSBP(CurrentStackPTR)))
      {
	if((DLword*)CURRENTFX >= CurrentStackPTR)
	  {
	    WARN("CURRENTFX >= CurrentStackPTR??\n",
	    printf("CURRENTFX=0x%x,CurrentStackPTR=0x%x\n",
		    LADDR_from_68k(CURRENTFX),LADDR_from_68k(CurrentStackPTR)));
	  }
	setflg=T;
	printf("set CURRENTFX->nextblock in debugger. But it will be reset after this check \n");
	save_nextblock=CURRENTFX->nextblock;
	savestack1= GETWORD(CurrentStackPTR+2);
	savestack2= GETWORD(CurrentStackPTR+3);
	CURRENTFX->nextblock=StkOffset_from_68K(CurrentStackPTR+2);
	GETWORD(CurrentStackPTR+2)=STK_FSB_WORD;
	GETWORD(CurrentStackPTR+3)= (((int)EndSTKP-(int)(CurrentStackPTR+2))>>1);
      }


    if(start68k) scanptr68k =start68k;
    else scanptr68k =
	  (StackWord *)Addr68k_from_StkOffset(InterfacePage->stackbase);
    endstack68k=(StackWord *)Addr68k_from_StkOffset(InterfacePage->endofstack);

    if(STKWORD(endstack68k)->flags !=STK_GUARD)
      printf("?? endstack is not GUARD BLK\n");

    while(scanptr68k <endstack68k)
      {
	switch((unsigned)(STKWORD(scanptr68k)->flags))
	  {
	    case STK_FSB :	freesize= FSB_size(scanptr68k);
				if (freesize == 0)
				  {
				    warn("FSB freesize = 0!");
				  }
#ifdef FSBCHECK
			if(freesize > STACKAREA_SIZE+MINEXTRASTACKWORDS){
			    if(bigFSBindex < 100){
				bigFSB[bigFSBindex].offset = StkOffset_from_68K(scanptr68k);
				bigFSB[bigFSBindex].size = freesize;
				bigFSBindex++;
			    }
			}
#endif
                        scanptr68k = (StackWord *)
					((DLword *)scanptr68k + freesize);
					putchar('F');
					break;
	     case STK_GUARD:	freesize = FSB_size(scanptr68k);
				if (freesize == 0)
				  {
				    warn("Guard block freesize = 0!");
				  }
				scanptr68k = (StackWord *)
					((DLword *)scanptr68k + freesize);
				putchar('G');
				break;
        case STK_FX :	CHECK_FX(scanptr68k);
					scanptr68k =(StackWord *)Addr68k_from_StkOffset(
					((FX*)scanptr68k)->nextblock);
					putchar('X');
					break;
	   default:		top_ivar=(DLword*)scanptr68k;
					while(STKWORD(scanptr68k)->flags != STK_BF) {
						if (STKWORD(scanptr68k)->flags!=STK_NOTFLG)
						{
                                WARN("StackCheck:!=STK_NOTFLG",printf("content:0x%x\n",GETWORD(scanptr68k)));
						}
				scanptr68k = (StackWord *)
					((DLword *)scanptr68k + DLWORDSPER_CELL);
                           }/* while end */;
					CHECK_BF(scanptr68k);
					if(((Bframe*)scanptr68k)->residual)
					 {
						if((DLword*)scanptr68k != top_ivar)
							printf("Residual has real IVAR:0x%x\n",LADDR_from_68k(scanptr68k));
					  }
					else
					 {
					   if(((Bframe*)scanptr68k)->ivar
						!= StkOffset_from_68K(top_ivar))
						printf("BF doesn't point TopIVAR:0x%x\n",LADDR_from_68k(scanptr68k));
					  }
					scanptr68k = (StackWord *)
							((DLword *)scanptr68k
					      		  + DLWORDSPER_CELL);
					putchar('B');
					break;
  }/*switch end */
  if(scanptr68k != start68k)
  {
	if(scanptr68k >endstack68k)
	{
		WARN("scanptr exceeded end stack",printf("scanptr68k=0x%x endstack68k=0x%x",scanptr68k,endstack68k));
	}
  }
}/* while end */
#ifdef FSBCHECK
if(bigFSBindex!=0){
   int i;

   printf("\nBIG FSB(s):\n");

   for(i=0;i<bigFSBindex;i++){
	printf("Offset: 0x%x , Size: 0x%x\n",bigFSB[i].offset,bigFSB[i].size);
   }
}
#endif
printf("\nStack Check done\n");
 if(setflg)
 {
	CURRENTFX->nextblock=save_nextblock;
	GETWORD(CurrentStackPTR+2)=savestack1;
	GETWORD(CurrentStackPTR+3)=savestack2;
 }

}/*stack_check end */



/**************************************************************/
/*
		quick_stack_check()
		for DEBUGING using FNSTKCHECK
*/
/**************************************************************/
quick_stack_check()
  {
    StackWord *start68k;
    StackWord *scanptr68k;
    StackWord *endstack68k;
    DLword *top_ivar;
    DLword save_nextblock;
    DLword savestack1,savestack2;
    DLword setflg=NIL;
    DLword freesize;

#ifdef FSBCHECK
    struct big_fsbs {   DLword offset; DLword size;} bigFSB[100];
    int bigFSBindex=0;

    bzero((char*)bigFSB, 100 * 2 * 2);
#endif

    if((CURRENTFX->nextblock != StkOffset_from_68K(CurrentStackPTR))
	||(!FSBP(CurrentStackPTR)))
      {
	if((DLword*)CURRENTFX >= CurrentStackPTR)
	  {
		return(0);
	    WARN("CURRENTFX >= CurrentStackPTR??\n",
	    printf("CURRENTFX=0x%x,CurrentStackPTR=0x%x\n",
		    LADDR_from_68k(CURRENTFX),LADDR_from_68k(CurrentStackPTR)));
	  }
	setflg=T;
	save_nextblock=CURRENTFX->nextblock;
	savestack1= GETWORD(CurrentStackPTR+2);
	savestack2= GETWORD(CurrentStackPTR+3);
	CURRENTFX->nextblock=StkOffset_from_68K(CurrentStackPTR+2);
	GETWORD(CurrentStackPTR+2)=STK_FSB_WORD;
	GETWORD(CurrentStackPTR+3)= (((int)EndSTKP-(int)(CurrentStackPTR+2))>>1);
      }


    scanptr68k = start68k =
	  (StackWord *)Addr68k_from_StkOffset(InterfacePage->stackbase);
    endstack68k=(StackWord *)Addr68k_from_StkOffset(InterfacePage->endofstack);

    if(STKWORD(endstack68k)->flags !=STK_GUARD)
      printf("?? endstack is not GUARD BLK\n");

    while(scanptr68k <endstack68k)
      {
	switch((unsigned)(STKWORD(scanptr68k)->flags))
	  {
	    case STK_FSB :	freesize= FSB_size(scanptr68k);
				if (freesize == 0)
				  {
				    warn("FSB freesize = 0!");
				    return(1);
				  }
#ifdef FSBCHECK
			if(freesize > STACKAREA_SIZE+MINEXTRASTACKWORDS){
			    if(bigFSBindex < 100){
				bigFSB[bigFSBindex].offset = StkOffset_from_68K(scanptr68k);
				bigFSB[bigFSBindex].size = freesize;
				bigFSBindex++;
			    }
			}
#endif
                        scanptr68k = (StackWord *)
					((DLword *)scanptr68k + freesize);
					break;
	     case STK_GUARD:	freesize = FSB_size(scanptr68k);
				if (freesize == 0)
				  {
				    warn("Guard block freesize = 0!");
				    return(1);
				  }
				scanptr68k = (StackWord *)
					((DLword *)scanptr68k + freesize);
				break;
        case STK_FX :	CHECK_FX(scanptr68k);
					scanptr68k =(StackWord *)Addr68k_from_StkOffset(
					((FX*)scanptr68k)->nextblock);
					break;
	   default:		top_ivar=(DLword*)scanptr68k;
					while(STKWORD(scanptr68k)->flags != STK_BF) {
						if (STKWORD(scanptr68k)->flags!=STK_NOTFLG)
						{
                                warn("StackCheck:!=STK_NOTFLG");
				printf("content:0x%x\n",GETWORD(scanptr68k));
				return(1);
						}
				scanptr68k = (StackWord *)
					((DLword *)scanptr68k + DLWORDSPER_CELL);
                           }/* while end */;
					CHECK_BF(scanptr68k);
					if(((Bframe*)scanptr68k)->residual)
					 {
						if((DLword*)scanptr68k != top_ivar)
							printf("Residual has real IVAR:0x%x\n",LADDR_from_68k(scanptr68k));
					  }
					else
					 {
					   if(((Bframe*)scanptr68k)->ivar
						!= StkOffset_from_68K(top_ivar))
						printf("BF doesn't point TopIVAR:0x%x\n",LADDR_from_68k(scanptr68k));
					  }
					scanptr68k = (StackWord *)
							((DLword *)scanptr68k
					      		  + DLWORDSPER_CELL);
					break;
  }/*switch end */
  if(scanptr68k != start68k)
  {
	if(scanptr68k >endstack68k)
	{
		WARN("scanptr exceeded end stack",printf("scanptr68k=0x%x endstack68k=0x%x",scanptr68k,endstack68k));
	}
  }
}/* while end */
#ifdef FSBCHECK
if(bigFSBindex!=0){
   int i;

   printf("\nBIG FSB(s):\n");

   for(i=0;i<bigFSBindex;i++){
	printf("Offset: 0x%x , Size: 0x%x\n",bigFSB[i].offset,bigFSB[i].size);
   }
}
#endif

 if(setflg)
 {
	CURRENTFX->nextblock=save_nextblock;
	GETWORD(CurrentStackPTR+2)=savestack1;
	GETWORD(CurrentStackPTR+3)=savestack2;
 }
 return(0);

}/* quick_stack_check end */






check_FX(fx68k)
FX *fx68k;
{
	Bframe *dummybf,*mtmp;
	int mblink;
	if(((FX*)(fx68k))->flags != STK_FX)
		error("CheckFX:NOT FX");
	dummybf=(Bframe*)DUMMYBF(fx68k);
	if(StkOffset_from_68K(dummybf) == GETBLINK(fx68k))
			{goto checkfx_OK;}
	mblink=GETBLINK(fx68k);
	mtmp=(Bframe*)Addr68k_from_StkOffset(mblink);
	if( ( dummybf->residual != NIL ) &&
		( dummybf->ivar == mtmp->ivar ) )
			goto checkfx_OK;
	else
		error("CheckFX:bad residual case");
	checkfx_OK: return;
} /* END check_FX */



check_BF(bf68k)
 Bframe *bf68k;
{ Bframe *iptr68k;

	if(bf68k->flags != STK_BF)
		error("checkBF:not BF1");
	if(bf68k->residual == T)
		return;
	else
	{
	for(iptr68k=(Bframe*)Addr68k_from_StkOffset(bf68k->ivar);
		iptr68k <=(Bframe*)(((DLword*)bf68k)-2);
		iptr68k++ ) /* inc 2DLword */
		{
		  if(iptr68k->flags!=STK_NOTFLG)
			error("CheckBF:Not BF2");
		}
	}

} /* end check_BF */


/************************************************************************/
/*									*/
/*									*/
/*									*/
/*									*/
/*									*/
/************************************************************************/

check_stack_rooms(fx68k)
  FX *fx68k;
  {
    int size;
    DLword *freeptr68k,*endstk68k;

    CHECK_FX(fx68k);
    freeptr68k=Addr68k_from_StkOffset(fx68k->nextblock);
    if(!FSBP(freeptr68k))
    error("check_stack_rooms:  nextblock doesn't point to an FSB");
    /*endstk68k=freeptr68k + FSB_size(freeptr68k);
    size=((int)endstk68k - (int)CurrentStackPTR) >> 1;*/
    return(FSB_size(freeptr68k));

  } /* end check_stack_rooms */
