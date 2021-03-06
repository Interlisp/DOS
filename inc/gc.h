/* @(#) gc.h Version 2.13 (4/21/92). copyright venue & Fuji Xerox  */

/*
 *
 *	Author	:	Takeshi Shimizu
 *			Hiroshi Hayata	
 */

/************************************************************************/
/*									*/
/*	Copyright 1989, 1990 Venue, Fuji Xerox Co., Ltd, Xerox Corp.	*/
/*									*/
/*	This file is work-product resulting from the Xerox/Venue	*/
/*	Agreement dated 18-August-1989 for support of Medley.		*/
/*									*/
/************************************************************************/

/**********************************************************************/
/*
 		File Name :  	gc.h

		Define  for garbage collector	
 
*/
/**********************************************************************/
#define	ADDREF	0	/* for gclookup routine. */
#define	DELREF	1	/* for gclookup routine. */
#define	STKREF	2	/* for gclookup routine. */
#define UNSTKREF	3	/* for htfind function. (T.T.) */

#define MAX_GCCOUNT	0x3F 	/* = 63 */

/* IncAllocCnt(n) decrements reclaim countdown by N and signals interrupt
   if GC should happen soon */

/* IncAllocCnt is called only when *Reclaim_cnt_word != NIL */
#define IncAllocCnt(n) {\
	if ((*Reclaim_cnt_word -= n) <= S_POSITIVE) {\
		/* time for GC */\
		Irq_Stk_Check = Irq_Stk_End = 0;\
		*Reclaim_cnt_word = S_POSITIVE;\
	};\
}

/* DecAllocCnt only called when *Reclaim_cnt_word != NIL */
#define DecAllocCnt(n) { *Reclaim_cnt_word += n; }

#define FreeLink(link) {\
	GETWORD(link) = 0;\
	GETWORD(link+1) = GETWORD(HTcoll);\
	GETWORD(HTcoll) = (link - HTcoll);\
}

#define GetLinkptr(entry)	(entry & 0x0fffe)

#define DelLink(link, prev, entry) {					     \
	if (prev != (DLword *)0) {					     \
		GETWORD((DLword *)prev + 1) = GETWORD((DLword *)link + 1);   \
	} else {							     \
		GETWORD((DLword *)entry) = GETWORD((DLword *)link + 1) | 1;  \
	}								     \
	FreeLink((DLword *)link);					     \
	link = (DLword *)HTcoll + GetLinkptr(GETWORD((DLword *)entry));	     \
	if (GETWORD((DLword *)link + 1) == 0) {			     	     \
		GETWORD((DLword *)entry) = GETWORD((DLword *)link);	     \
		FreeLink((DLword *)link);				     \
	}								     \
}

#define RefCntP(ptr) (!(GetTypeEntry((ptr)) & TT_NOREF) &&		     \
		      (*GcDisabled_word != ATOM_T))

#define GCLOOKUP(ptr, case) {						     \
	if (RefCntP(ptr)) {						     \
		if (*Reclaim_cnt_word != NIL)				     \
		  htfind(ptr, case);					     \
		else							     \
		  rec_htfind(ptr, case);				     \
	}							             \
}

#define GCLOOKUPV(ptr, case, val) {					     \
	if (RefCntP(ptr)) {						     \
		if (*Reclaim_cnt_word != NIL)				     \
		  val = htfind(ptr, case);				     \
		else							     \
		  val = rec_htfind(ptr, case);				     \
	} else val = NIL;						     \
}

#define REC_GCLOOKUP(ptr, case) { if (RefCntP(ptr)) rec_htfind(ptr, case); }
#define REC_GCLOOKUPV(ptr, case, val) {					     \
	if (RefCntP(ptr)) 						     \
	  val = rec_htfind(ptr, case);					     \
	else								     \
	  val = NIL;							     \
}

#define FRPLPTR(old , new) { \
		GCLOOKUP(new, ADDREF); \
		GCLOOKUP(old, DELREF); \
		(old) = (new) ; }


#ifndef BYTESWAP
	/********************************************************/
	/*   Normal byte-order definitions, for e.g., 68020s	*/
	/********************************************************/

struct	 hashentry
  {
    unsigned	count		:6;
    unsigned	stackref	:1;
    unsigned	segnum		:8;
    unsigned	collision	:1;
  };

struct  htlinkptr
  {	/* overlay access method */
    DLword	contents;
  };

struct	htcoll
  {
    DLword	free_ptr ;
    DLword	next_free ;
  };

struct	gc_ovfl
  {
    LispPTR	  ovfl_ptr ;
    unsigned int  ovfl_cnt ;
  };

struct  htoverflow 
  {	/* July-23-1987 by TT */
    unsigned	pcase	:8;
    unsigned	ptr	:24;
  };

#else

	/********************************************************/
	/*	Byte-swapped definitions, for e.g., 80386s	*/
	/********************************************************/

struct	 hashentry
  {
    unsigned	collision	:1;
    unsigned	segnum		:8;
    unsigned	stackref	:1;
    unsigned	count		:6;
  };

struct  htlinkptr
  {	/* overlay access method */
    DLword	contents;
  };

struct	htcoll
  {
    DLword	next_free ;
    DLword	free_ptr ;
  };

struct	gc_ovfl
  {
    LispPTR	  ovfl_ptr ;
    unsigned int  ovfl_cnt ;
  };

struct  htoverflow 
  {	/* July-23-1987 by TT */
    unsigned	ptr	:24;
    unsigned	pcase	:8;
  };


#endif /* BYTESWAP */

