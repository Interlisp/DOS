/* @(#) gchtfind.c Version 2.13 (4/21/92). copyright venue & Fuji Xerox  */
static char *id = "@(#) gchtfind.c	2.13 4/21/92		(venue & Fuji Xerox)";

#include "lispemul.h"
#include "lsptypes.h"
#include "address.h"
#include "adr68k.h"
#include "lspglob.h"
#include "gc.h"
#include "lispmap.h"
#include "cell.h"

#define Evenp(num,prim) ((num % prim) == 0)
#define HTCOLLMAX		HTCOLL_SIZE-16

/* GetLink gets a new entry from the GC collision table */
#define GetLink(var) {							    \
									    \
	register DLword linkoff;					    \
	linkoff = GETWORD(HTcoll);					    \
	if (linkoff == 0) {						    \
		if ((linkoff = GETWORD((DLword *)HTcoll + 1))		    \
		    >= HTCOLLMAX) {					    \
			disablegc1(NIL);				    \
			return(NIL);  					    \
		};							    \
		GETWORD(HTcoll + 1) = linkoff + 2;			    \
		var = (DLword *)HTcoll + linkoff;			    \
	} else {							    \
		GETWORD(HTcoll) = GETWORD((DLword *)HTcoll + linkoff + 1);  \
		var = (DLword *)HTcoll + linkoff;			    \
	}								    \
}


#define HTCNTSHIFT 10	/* amount to shift to get hash table count */
#define HTCNTMASK 0xFC00 /* mask which masks off hash table count */
#define HTCNTSTKMASK 0XFE00 /* mask for hash table count + stack bit */
#define HTSTKMASK 0x0200 /* mask for stack bit only */
#define HTHIMASK 0x1FE	/* mask of bits which contain high part of pointer
			in hash table */
#define HTHISHIFT 1  /* high bits in hash table are shifted left 1 */


/*  NewEntry is a macro for adding a new gc hash table entry;
	entry is pointer to hash table entry
	hiptr is the high point of the ref-cnted entry, shifted
	casep is one of ADDREF, DELREF, etc.
*/

/*
 * NewEntry is never called in the course of the reclaimation.
 * Thus STKREF case is not needed.
 */
#define NewEntry(entry, hiptr, casep, ptr) {				     \
switch (casep) {							     \
	case ADDREF:							     \
	    GETWORD(entry) = hiptr | (2 << HTCNTSHIFT);	/* set count = 2 */  \
	    IncAllocCnt(1);\
	    return NIL;				/* not new 0 entry */	     \
	case DELREF:							     \
	    GETWORD(entry) = hiptr;		/* set count = 0 */	     \
	    IncAllocCnt(1);\
	    return ptr;				/* new 0 entry */	     \
	default:							     \
	    error("GC error: new entry touches stack bit");	     \
	}}

/*
 * RecNewEntry is called in the course of the reclaimation.
 * Does not maintain the allocation count.
 */
#define RecNewEntry(entry, hiptr, casep, ptr) {				     \
switch (casep) {							     \
	case ADDREF:							     \
	    GETWORD(entry) = hiptr | (2 << HTCNTSHIFT);	/* set count = 2 */  \
	    return NIL;				/* not new 0 entry */	     \
	case DELREF:							     \
	    GETWORD(entry) = hiptr;		/* set count = 0 */	     \
	    return ptr;				/* new 0 entry */	     \
	case STKREF:  /* set refcnt to 1, stack bit to 1 */		     \
	    GETWORD(entry) = hiptr | (1<<HTCNTSHIFT) | HTSTKMASK;	     \
	    return NIL;							     \
	default:							     \
	    error("GC error: new entry when turning off stack bit");	     \
	}}

/* ModEntry is a macro to modify an old gc hash table entry.
	entry is a pointer to the entry
	contents holds the old contents
	ptr is the pointer being counted
	casep is one of ADDREF, DELREF, etc.
	remove is a label to go to if the entry will go away

	It always return NIL, since cannot be creating a zero-count,
	 no-stack-bit entry */
/*
 * ModEntry is never called in the course of the reclamation.
 * Thus STKREF and UNSTKREF cases are not needed.
 */
#define ModEntry(entry, contents, ptr, casep, remove) { \
	if (contents >= HTCNTMASK) { /* overflow; return non-zero */ \
		modify_big_reference_count(entry, casep, ptr);\
		return NIL;}\
	switch (casep) {\
	case ADDREF:\
		contents += (1 << HTCNTSHIFT);\
		if (contents >= HTCNTMASK ) {\
			/* overflow */\
			GETWORD(entry) = contents; \
			enter_big_reference_count(ptr);\
			return NIL;\
			}\
         	if ((contents & HTCNTSTKMASK) == (1 << HTCNTSHIFT)) { \
                	DecAllocCnt(1);\
         		goto remove;\
         	}\
		break;\
	case DELREF:\
		if ( (contents -= (1 << HTCNTSHIFT)) < 0) \
			error("attempt to decrement 0 reference count");\
         	if ((contents & HTCNTSTKMASK) == (1 << HTCNTSHIFT)) { \
                	DecAllocCnt(1);\
         		goto remove;\
         	}\
		break;\
	default:							     \
	    error("GC error: mod entry touches stack bit");	     \
	}\
	GETWORD(entry) = contents;\
	return NIL;\
}

/*
 * RecModEntry is called in the course of the reclaimation.
 * Does not maintain the allocation count.
 */
#define RecModEntry(entry, contents, ptr, casep, remove) { \
	if (contents >= HTCNTMASK) { /* overflow; return non-zero */ \
		modify_big_reference_count(entry, casep, ptr);\
		return NIL;}\
	switch (casep) {\
	case ADDREF:\
		contents += (1 << HTCNTSHIFT);\
		if (contents >= HTCNTMASK ) {\
			/* overflow */\
			GETWORD(entry) = contents; \
			enter_big_reference_count(ptr);\
			return NIL;\
			}\
		break; /* check for possibly deleting entry */\
	case DELREF:\
		if ( (contents -= (1 << HTCNTSHIFT)) < 0) \
			error("attempt to decrement 0 reference count");\
		break;\
	case STKREF:\
		GETWORD(entry) = contents | HTSTKMASK;\
		return NIL;\
/*\
	case UNSTKREF:\
		contents = contents & ~ HTSTKMASK;\
		break;\
*/\
	}\
	if ((contents & HTCNTSTKMASK) == (1 << HTCNTSHIFT)) goto remove;\
	GETWORD(entry) = contents;\
	return NIL;\
}


/* add a new overflow entry for counts > 62 */

void enter_big_reference_count(ptr)
LispPTR			ptr;
   { struct gc_ovfl	*oventry;
     register LispPTR	tmp;

	/* this kludge is apparently necessary. Odd pointers are
	illegal, but apparently some are reference counted. If you
	get an odd pointer, just ignore the low bit */

	if(ptr&1) ptr &= 0xfffffffe;
	oventry = (struct gc_ovfl *)HTbigcount;
	while(((tmp = oventry->ovfl_ptr) != ATOM_T) && (tmp != NIL))
						/* free area ? */
	       {if (tmp == ptr)
			{ error(
			"ERROR : PTR already in overflow table.\n");
			/* "Assure it lives forever" */
			  oventry->ovfl_cnt += 0x10000;
			  return;
			}
		else ++oventry;
		};
	if (tmp == NIL) {
		if (Evenp(LADDR_from_68k(oventry+1),DLWORDSPER_PAGE)) {
			if ((unsigned int)oventry+1 >= (unsigned int)HTcoll)
				error("GC big reference count table overflow");
			newpage(LADDR_from_68k(oventry+1));
			}
		}

	oventry->ovfl_cnt   = MAX_GCCOUNT;
	oventry->ovfl_ptr   = ptr;
	return;
   }

/* modify an old overflow entry */

void modify_big_reference_count(entry, casep, ptr)
	DLword          casep;
	DLword         *entry;
	LispPTR         ptr;
{
	struct gc_ovfl *oventry;
	LispPTR         tmp;
	int             tmpcnt;

	/* ditto comment in entry_big_reference_count */
	if (ptr & 1) ptr &= 0xfffffffe;
	oventry = (struct gc_ovfl *) HTbigcount;
	while ((tmp = oventry->ovfl_ptr) != ptr)
		if (tmp == NIL) {
			error(
			      "refcnt previously overflowed, but not found in table.\n");
			return;
		} else
			++oventry;	/* increment by size of oventry
					 * structure */
	switch (casep) {
	case ADDREF:
		++(oventry->ovfl_cnt);
		return;
	case DELREF:
		if (--(oventry->ovfl_cnt) < MAX_GCCOUNT) {
			/* fallen below threshold */
			((struct hashentry *) WORDPTR(entry))->count = MAX_GCCOUNT - 1;
			oventry->ovfl_ptr = ATOM_T;
		}
		return;
	case STKREF:
		((struct hashentry *) WORDPTR(entry))->stackref = 1;
		return;
/*
	case UNSTKREF:
		((struct hashentry *) WORDPTR(entry))->stackref = 0;
		return;
*/
	}
}




/* htfind is the main entry for reference manipulation.
  It modifies the reference count of the Lisp pointer ptr.

   casep is one of ADDREF, DELREF, STKREF

	ADDREF = add 1
	DELREF = subtract 1
	STKREF = turn on stack bit
	UNSTKREF = turn off stack bit

   returns NIL if DELREF and the entry became
	refcount = 0, stk bit off (only can happen on a *new* DELREF)
	in which case it returns PTR */

LispPTR htfind(ptr, casep)
	LispPTR         ptr;
	int             casep;
{
	register DLword *entry, *link, *prev;
	register DLword entry_contents, hiptr;
	register struct htlinkptr *htlptr;

	/* if the NOREF bit is on in the type table entry, do
	not reference count this pointer. Used for non-reference
	counted types like symbols, and also when the GC is
	disabled. */

	/*
	 * Following two tests were moved into GCLOOKUP macro
	 * for efficency.
	 */

/*
	if (GetTypeEntry(ptr) & TT_NOREF)
		return NIL;
*/
	/* if *GcDisabled_word is T then do nothing */
	/* FS:  this test should not be needed (because type table should
		be cleared).  Also, this test seems to cause an infinite
		ucode loop in remimplicitkeyhash on the 386i		*/

/*	if(*GcDisabled_word == ATOM_T) return(NIL); */

	/* GC hash table entries have the high 8 bits of the
	pointer stored in the middle. Set up hiptr to have
	the high bits of the pointer ready to store or test
	against */

	hiptr = (((unsigned int)ptr) >> (16-HTHISHIFT) ) & HTHIMASK;

	/* entry points at the place in the main hash table
	where this pointer is stored. The 'hash' isn't one really;
	it just uses the low bits of the pointer. */

	entry = HTmain + (LOLOC(ptr) >> 1);

	entry_contents = GETWORD(entry);

	if (entry_contents == 0)
		NewEntry(entry, hiptr, casep, ptr);
		/* NewEntry returns */

	if (entry_contents & 1) {	/* low bit means a collision entry */
		/* entry_contents-1 removes low bit */
		link = HTcoll + (entry_contents - 1);
		prev = 0;
		goto newlink;
	}

	if (hiptr == (entry_contents & HTHIMASK)) {
		ModEntry(entry, entry_contents, ptr, casep, delentry);
		/* ModEntry returns or will go to delentry */
	}

	/* new collision */

	GetLink(link);
	GetLink(prev);
	GETWORD((DLword *)prev + 1) = 0;
	GETWORD((DLword *)prev) = entry_contents;
	GETWORD((DLword *)link + 1) = prev - HTcoll;
	GETWORD((DLword *)entry) = link - HTcoll + 1;

	NewEntry(link, hiptr, casep, ptr);


delentry: GETWORD(entry) = 0;
	return NIL;

/* start here when a collision is detected. link is a pointer to
  the entry in the collision table, prev is the previous collision
  entry or 0 if this is the first one. */

newlink:entry_contents = GETWORD(link);
	if (hiptr == (entry_contents & HTHIMASK) ) {
		ModEntry(link, entry_contents, ptr, casep, dellink);
		/* ModEntry returns or goes to dellink */
	}
	/* collision didn't match  */
	entry_contents = GETWORD((DLword *)link + 1);
	if (entry_contents == 0) {
		goto nolink;
	}
	/* try the next link in the collision table */
	prev = link;
	link = HTcoll + entry_contents;
	goto newlink;

dellink:if (prev)
		GETWORD((DLword *)prev + 1) = GETWORD((DLword *)link + 1);
	else
		GETWORD((DLword *)entry) = (GETWORD((DLword *)link + 1)) | 1;
	FreeLink(link);
	link = HTcoll + (GETWORD((DLword *)entry)) - 1;
	if (GETWORD(link + 1) == 0) {
		GETWORD((DLword *)entry) = GETWORD((DLword *)link);
		FreeLink(link);
	}
	return NIL;

nolink:			/* no match */

	GetLink(link);
	GETWORD((DLword *)link + 1) = GETWORD((DLword *)entry) - 1;
	GETWORD((DLword *)entry) = (link - HTcoll) + 1;
	NewEntry(link, hiptr, casep, ptr);


}

LispPTR rec_htfind(ptr, casep)
	LispPTR         ptr;
	int             casep;
{
	register DLword *entry, *link, *prev;
	register DLword entry_contents, hiptr;
	register struct htlinkptr *htlptr;

	/* if the NOREF bit is on in the type table entry, do
	not reference count this pointer. Used for non-reference
	counted types like symbols, and also when the GC is
	disabled. */

	/*
	 * Following two tests were moved into GCLOOKUP macro
	 * for efficency.
	 */
/*
	if (GetTypeEntry(ptr) & TT_NOREF)
		return NIL;
*/
	/* if *GcDisabled_word is T then do nothing */
	/* FS:  this test should not be needed (because type table should
		be cleared).  Also, this test seems to cause an infinite
		ucode loop in remimplicitkeyhash on the 386i		*/

/*	if(*GcDisabled_word == ATOM_T) return(NIL); */

	/* GC hash table entries have the high 8 bits of the
	pointer stored in the middle. Set up hiptr to have
	the high bits of the pointer ready to store or test
	against */

	hiptr = (((unsigned int)ptr) >> (16-HTHISHIFT) ) & HTHIMASK;

	/* entry points at the place in the main hash table
	where this pointer is stored. The 'hash' isn't one really;
	it just uses the low bits of the pointer. */

	entry = HTmain + (LOLOC(ptr) >> 1);

	entry_contents = GETWORD(entry);

	if (entry_contents == 0)
		RecNewEntry(entry, hiptr, casep, ptr);
		/* NewEntry returns */

	if (entry_contents & 1) {	/* low bit means a collision entry */
		/* entry_contents-1 removes low bit */
		link = HTcoll + (entry_contents - 1);
		prev = 0;
		goto newlink;
	}

	if (hiptr == (entry_contents & HTHIMASK)) {
		RecModEntry(entry, entry_contents, ptr, casep, delentry);
		/* ModEntry returns or will go to delentry */
	}

	/* new collision */

	GetLink(link);
	GetLink(prev);
	GETWORD((DLword *)prev + 1) = 0;
	GETWORD((DLword *)prev) = entry_contents;
	GETWORD((DLword *)link + 1) = prev - HTcoll;
	GETWORD((DLword *)entry) = link - HTcoll + 1;

	RecNewEntry(link, hiptr, casep, ptr);


delentry: GETWORD(entry) = 0;
	return NIL;

/* start here when a collision is detected. link is a pointer to
  the entry in the collision table, prev is the previous collision
  entry or 0 if this is the first one. */

newlink:entry_contents = GETWORD(link);
	if (hiptr == (entry_contents & HTHIMASK) ) {
		RecModEntry(link, entry_contents, ptr, casep, dellink);
		/* ModEntry returns or goes to dellink */
	}
	/* collision didn't match  */
	entry_contents = GETWORD(link + 1);
	if (entry_contents == 0) {
		goto nolink;
	}
	/* try the next link in the collision table */
	prev = link;
	link = HTcoll + entry_contents;
	goto newlink;

dellink:if (prev)
		GETWORD((DLword *)prev + 1) = GETWORD((DLword *)link + 1);
	else
		GETWORD((DLword *)entry) = (GETWORD((DLword *)link + 1)) | 1;
	FreeLink(link);
	link = HTcoll + (GETWORD((DLword *)entry)) - 1;
	if (GETWORD((DLword *)link + 1) == 0) {
		GETWORD((DLword *)entry) = GETWORD((DLword *)link);
		FreeLink(link);
	}
	return NIL;

nolink:			/* no match */

	GetLink(link);
	GETWORD((DLword *)link + 1) = GETWORD((DLword *)entry) - 1;
	GETWORD((DLword *)entry) = (link - HTcoll) + 1;
	RecNewEntry(link, hiptr, casep, ptr);


}


