/* This is G-file @(#) gcmain3.c Version 2.7 (4/21/92). copyright Venue & Fuji Xerox  */
static char *id = "@(#) gcmain3.c	2.7 4/21/92";



/************************************************************************/
/*									*/
/*	Copyright 1989, 1990 Venue, Fuji Xerox Co., Ltd, Xerox Corp.	*/
/*									*/
/*	This file is work-product resulting from the Xerox/Venue	*/
/*	Agreement dated 18-August-1989 for support of Medley.		*/
/*									*/
/************************************************************************/

/*************************************************************************/
/*                                                                       */
/*                       File Name : gcmain3.c                           */
/*                                                                       */
/*************************************************************************/
/*                                                                       */
/*                      Creation Date : July-7-1987                      */
/*                      Written by Tomoru Teruuchi                       */
/*                                                                       */
/*************************************************************************/
/*                                                                       */
/*           Functions : gcmapscan();                                    */
/*                       gcmapunscan();                                  */
/*                       gcscanstack();                                  */
/*                                                                       */
/*                                                                       */
/*************************************************************************/
/*           Descreption :                                               */
/*                                                                       */
/*************************************************************************/
/*                                                               \Tomtom */
/*************************************************************************/

#include "lispemul.h"
#include "lsptypes.h"
#include "address.h"
#include "adr68k.h"
#include "lspglob.h"
#include "emlglob.h"
#include "stack.h"
#include "cell.h"
#include "ifpage.h"
#include "gc.h"

#define HTMAIN_SIZE	0x8000   /* this will be deleted in including map */
#define STK_HI		1	/* This is also. */
#define WORDSPERCELL	2
#define CONSPAGE_LAST	0x0ffff
#define MAXHTCNT	63
#define PADDING	4
#define FNOVERHEADWORDS	8
#define BIND_BITS(value)	((unsigned int) (value) >> 24)
#define BF_FLAGS(value)		((unsigned int) (value) >> 29)
#define PTR_BITS(entry)		((unsigned int)((unsigned int)((entry) << 8) >> 8))
#define ADD_OFFSET(ptr, dloffset) ((LispPTR *) ((DLword *) (ptr) + (dloffset)))
#define GetSegnuminColl(entry1) 	((entry1 & 0x01fe) >> 1)
#define GetCountinColl(entry1) 	((entry1 & 0x0fc00) >> 10)
#define GetStkCnt(entry1)	((entry1 & 0x0fe00) >> 9)
#define GetStkrefinColl(entry1)	((entry1 & 0x0200) >> 9)
#define StkCntIsZero(entry1)	(!(entry1 & 0x0fe00))
#define StkrefP(entry1)		(entry1 & 0x0200)
#define SinglerefP(entry1)	((entry1 & 0xFC00) == 0x0400)
#define Boundp(frame_field)	((frame_field) == 0)
#define Stkref(ptr)		REC_GCLOOKUP(ptr, STKREF)
#define GcreclaimLp(ptr) 	while((ptr = gcreccell(ptr)) != NIL)  \
				  REC_GCLOOKUP(ptr, ADDREF)
#define HTLPTR ((struct htlinkptr *) WORDPTR(entry))
#define HENTRY ((struct hashentry *) WORDPTR(entry))


gcmapscan()
{
	register DLword			probe;
	register DLword			*entry;
	DLword				offset;
	register LispPTR		ptr;

	probe = HTMAIN_SIZE;
      nextentry:
	while ((probe = gcscan1(probe)) != NIL) {
		entry = (DLword *)HTmain + probe;
	      retry:
		if (HENTRY->collision) {
			register DLword	*prev;
			register DLword	*link;
		
			prev = (DLword *)0;
			link = (DLword *)HTcoll + GetLinkptr(HTLPTR->contents);
		      linkloop:
			offset = ((struct htcoll *)link)->free_ptr;
			if (StkCntIsZero(offset)) {
				/* Reclaimable object */
				ptr = VAG2(GetSegnuminColl(offset), (probe << 1));
				DelLink(link, prev, entry);
				GcreclaimLp(ptr);
				if (HTLPTR->contents == 0)
				  goto nextentry;
				else
				  goto retry;
			}
			if ((offset = ((struct htcoll *)link)->next_free)) {
				prev = link;
				link = (DLword *)HTcoll + offset;
				goto linkloop;
			}
			goto nextentry;
		}
		if (StkCntIsZero(HTLPTR->contents)) {
			ptr = VAG2(HENTRY->segnum, (probe << 1));
			HTLPTR->contents = 0;
			GcreclaimLp(ptr);
		}
	}
	return(NIL);
}

gcmapunscan()
{
	register DLword			probe;
	register DLword			*entry;
	DLword				offset;
	register LispPTR		ptr;

	probe = HTMAIN_SIZE;
	while ((probe = gcscan2(probe)) != NIL) {
		entry = (DLword *)HTmain + probe;
	      retry:
		if (HENTRY->collision) {
			register DLword	*prev;
			register DLword *link;

			prev = (DLword *)0;
			link = (DLword *)HTcoll + GetLinkptr(HTLPTR->contents);
		      scnlp:
			offset = ((struct htcoll *)link)->free_ptr;
			if (StkrefP(offset)) {
				if (SinglerefP(offset)) {
					DelLink(link, prev, entry);
					goto retry;
				} else {
					GETWORD((DLword *)link) =
					  offset & 0x0fdff;
				}
			}
			if ((offset = ((struct htcoll *)link)->next_free)) {
				prev = link;
				link = (DLword *)HTcoll + offset;
				goto scnlp;
			}
		} else if (HENTRY->stackref) {
			if (HENTRY->count == 1)
			  HTLPTR->contents = 0;
			else
			  HENTRY->stackref = 0;
		}
	}
	return(NIL);
}

gcscanstack()
	{ 
register  Bframe		*bascframe;
	  LispPTR		scanptr,scanend,scanend68K;
	  struct fnhead		*nametable;
	  int			ftyp;
	  int			pvcount;

  scanptr = VAG2(STK_HI,InterfacePage->stackbase);
  scanend = VAG2(STK_HI,InterfacePage->endofstack);
  scanend68K = (LispPTR) Addr68k_from_LADDR(scanend);
  bascframe = (Bframe *)Addr68k_from_LADDR(scanptr);
  while(1)/*This is endless loop until encountering tail of stack */ {
	ftyp = (int)bascframe->flags;
	switch(ftyp) {
		case STK_FX: { { 
			register struct frameex1 *frameex;
			register struct fnhead *fnheader;
			frameex = (struct frameex1 *)bascframe;
			scanptr = LADDR_from_68k(frameex);
			{ register LispPTR fn_head;
			  fn_head = (LispPTR)
				VAG2(frameex->hi2fnheader,
				     frameex->lofnheader);
			  Stkref(fn_head);
			  fnheader = (struct fnhead *)
					Addr68k_from_LADDR(fn_head);
			};
			{ register int pcou;
			  register LispPTR *pvars;
			  pvars =(LispPTR *)
				  ((DLword *)bascframe + FRAMESIZE);
			  for (pcou = fnheader->nlocals;pcou-- != 0;) {
				register LispPTR value;
				value = *pvars;
				if Boundp(BIND_BITS(value)) Stkref(value);
				++pvars;
			  }; /* for */
			}; /* register int pcou */

			{  register LispPTR qtemp;
			   register LispPTR next;
			   register LispPTR ntend;

			   next = qtemp = (LispPTR) 
				Addr68k_from_StkOffset(frameex->nextblock);
					 /* this is offset */
			   ntend = 0;	/* init flag */
			   if (frameex->validnametable) {
				register LispPTR nametable;
				register unsigned int hi2nametable;
				register unsigned int lonametable;
				lonametable = frameex->lonametable;
				hi2nametable = frameex->hi2nametable;
				nametable = VAG2(hi2nametable,lonametable);
				if (STK_HI == hi2nametable) {
	  	  	  		Stkref(fnheader->framename);
					qtemp = (LispPTR) 
						Addr68k_from_StkOffset(lonametable);
					ntend = (LispPTR) (
						((DLword *)qtemp) + 
						FNHEADSIZE +
						(((struct fnhead *)qtemp)->ntsize)*2);
				} else Stkref(nametable);
			  }; /* frameex->validnametable */
			  bascframe = (Bframe *) 
					ADD_OFFSET(bascframe, FRAMESIZE+PADDING+
						(((fnheader->pv)+1)<<2));
			scantemps:
			  while((unsigned int)bascframe < (unsigned int)qtemp) { 
				register LispPTR value;
				value = *((LispPTR *) bascframe);
				if Boundp(BIND_BITS(value))  Stkref(value);
				bascframe++;
				}; /* while */

			  if (ntend != 0) { 
				bascframe = (Bframe *)
					Addr68k_from_StkOffset(ntend);
				qtemp = next;
				ntend = 0;
				goto scantemps;
			  };
			  bascframe = (Bframe *) next;

			}; /* LOCAL regs qtemp next */
			}; /* local regs fnheader frameex */
		 	break;
			};
		case STK_GUARD:/* stack's tail ? */ {
			if ((unsigned int) bascframe >= (unsigned int) scanend68K)
				return(NIL);
		  	else { 
				bascframe = (Bframe *)((DLword *) 
				bascframe + bascframe->ivar);
				};
			break;
			};
		case STK_FSB: { 
			bascframe = (Bframe *)((DLword *) 
			bascframe + bascframe->ivar);
			break;
			};
		default:	/* must be basic frame !! */
			{
			register LispPTR bf_word;
			while(STK_BF != BF_FLAGS(bf_word = *((LispPTR *)bascframe)))
			{	Stkref(PTR_BITS(bf_word));
				bascframe++;
				};
			bascframe++;
			}; 


/* **** NOTE THIS CODE DOES NOT COMPILE CORRECTLY ON THE SUN 4
				{register LispPTR bf_word;
				 while(STK_BF != BF_FLAGS(
					bf_word = *((LispPTR *)bascframe++)))
					{ Stkref(PTR_BITS(bf_word));
					};
				}; 
**** */
		};	/* switch */
	};	/* while(1) */
}






