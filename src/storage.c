/* @(#) storage.c Version 2.14 (4/21/92). copyright venue & Fuji Xerox  */
static char *id = "@(#) storage.c	2.14 4/21/92		(venue & Fuji Xerox)";

/************************************************************************/
/*									*/
/*	Copyright 1989, 1990 Venue, Fuji Xerox Co., Ltd, Xerox Corp.	*/
/*									*/
/*	This file is work-product resulting from the Xerox/Venue	*/
/*	Agreement dated 18-August-1989 for support of Medley.		*/
/*									*/
/************************************************************************/

/*
 *	Copyright (C) 1987 by Fuji Xerox Co., Ltd. All rights reserved.
 *
 *	Author	:	Takeshi Shimizu
 */

/*****************************************************************/
/*
		File Name :	storage.c
		Including	:

		Created	:	Oct. 7, 1987 Takeshi Shimizu
		Changed :

*/
/*****************************************************************/

#include "hdw_conf.h"
#include "lispemul.h"
#include "address.h"
#include "adr68k.h"
#include "lispmap.h"
#include "stack.h"
#include "lspglob.h"
#include "cell.h"
#include "lsptypes.h"
#include "ifpage.h"
#include "gc.h"


#define MINARRAYBLOCKSIZE	4
#define GUARDVMEMFULL		500
#define IFPVALID_KEY		5603

/*****************************************************************/
/*
		Func Name :	checkfor_storagefull(npages)

		Created	:	Oct. 7, 1987 Takeshi Shimizu
		Changed :	Oct. 12,1987 take

*/
/*****************************************************************/


checkfor_storagefull(npages)
register  DLword npages;
{
 register int pagesleft;
 register INTSTAT *int_state;

 pagesleft = (*Next_MDSpage_word & 0xffff) - (*Next_Array_word & 0xffff) - PAGESPER_MDSUNIT ;

 if((pagesleft < GUARDSTORAGEFULL) || (npages !=0))
 {
	if (*STORAGEFULLSTATE_word == NIL)
	     set_storage_state();

	switch(*STORAGEFULLSTATE_word & 0xffff)
	 {
	    case SFS_NOTSWITCHABLE :
	    case SFS_FULLYSWITCHED : if(pagesleft <0) {
					while(T){
						error("MP9320:Storage completly full");
					}
				     }
				     else if ((pagesleft<=GUARD1STORAGEFULL)
						 && (*STORAGEFULL_word !=NIL))
					   {
						*STORAGEFULL_word = S_POSITIVE;
						error("MP9325:Space getting VERY full.\
							Please save and reload a.s.a.p.");
					   }
					  else if (*STORAGEFULL_word == NIL)
						{
						  *STORAGEFULL_word=ATOM_T;
						  int_state=(INTSTAT *)Addr68k_from_LADDR(*INTERRUPTSTATE_word);
						  int_state->storagefull=T;
						  *PENDINGINTERRUPT_word =ATOM_T;
						}
#ifdef  DEBUG
															     printf("\n checkfor_storagefull:DORECLAIM.....\n");
#endif
				     return(NIL);

				     break;

	    case SFS_SWITCHABLE    : if(npages==NIL)
				      {
					if(pagesleft <= 0)
					 {
						*LeastMDSPage_word= *Next_Array_word;
						*Next_MDSpage_word= *SecondMDSPage_word;
						advance_storagestate(SFS_FULLYSWITCHED);
						return(advance_array_seg((*SecondArrayPage_word) & 0xffff));
					 }
				      }
				     else if (npages > pagesleft)
					   {
						/* Have to switch array space over,
						  but leave MDS to fill the rest of the low pages   */
						*LeastMDSPage_word= *Next_Array_word;
						advance_storagestate(SFS_ARRAYSWITCHED);
						return(advance_array_seg((*SecondArrayPage_word) & 0xffff));
					   }
				     break ;

	    case SFS_ARRAYSWITCHED : if((*Next_MDSpage_word & 0xffff) <
					 (*LeastMDSPage_word & 0xffff))
				      {
					*Next_MDSpage_word= *SecondMDSPage_word;
					return(advance_storagestate
							(SFS_FULLYSWITCHED));
				      }
				     else if (npages != NIL)
					     if((npages + GUARDSTORAGEFULL) >=
						  ((*SecondMDSPage_word & 0xffff)-
						    (*Next_Array_word & 0xffff)))
							return(NIL);
				     return(T);
				  /* break; */

	   default :		     error("checkfor_storagefull: Shouldn't <%d>",(*STORAGEFULLSTATE_word) & 0xffff);
				     break;

	 }
 }
 else
	return(NIL);
}/* checkfor_storagefull end */

/*****************************************************************/
/*
		Func Name :	advance_array_seg(nxtpage)

		Created	:	Oct. 7, 1987 Takeshi Shimizu
		Changed :

*/
/*****************************************************************/

advance_array_seg(nxtpage)
 register DLword nxtpage; /* rare page num */
{

 DLword ncellsleft;
 LispPTR mergebackward();
 LispPTR makefreearrayblock();

  /* Called when 8Mb are exhausted,and we want to switch array space
   into the extended area(Secondary space),starting with nextpage.
   We MUST clean up old area first.   */

 ncellsleft =
	(*Next_Array_word -  POINTER_PAGE(*ArrayFrLst_word) -1)* CELLSPER_PAGE
	+ (CELLSPER_PAGE -  (((*ArrayFrLst_word) & 0xff) >> 1) );

 if(ncellsleft >= MINARRAYBLOCKSIZE)
  {
	mergebackward(makefreearrayblock(*ArrayFrLst_word , ncellsleft));
	*ArrayFrLst2_word = (((*LeastMDSPage_word) & 0xffff) << 8);
  }
else
  {
	*ArrayFrLst2_word = *ArrayFrLst_word;
  }

 *Next_Array_word = S_POSITIVE | nxtpage ;
 *ArrayFrLst_word = nxtpage << 8 ;
 *ArraySpace2_word = *ArrayFrLst_word;

 return(S_POSITIVE);

} /* advance_array_seg end */

/*****************************************************************/
/*
		Func Name :	advance_storagestate(flg)

		Created	:	Oct. 7, 1987 Takeshi Shimizu
		Changed :

*/
/*****************************************************************/

advance_storagestate(flg)
 DLword flg;
{
 LispPTR dremove();
#ifdef DEBUG
  printf("STORAGEFULLSTATE is now set to %d \n",flg);
#endif

 *STORAGEFULLSTATE_word = (S_POSITIVE | flg );
 InterfacePage->fullspaceused = 65535 ;
 *SYSTEMCACHEVARS_word = dremove(STORAGEFULLSTATE_index, *SYSTEMCACHEVARS_word);
}

/*****************************************************************/
/*
		Func Name :	set_storage_state()

		Created	:	Oct. 7, 1987 Takeshi Shimizu
		Changed :

*/
/*****************************************************************/
set_storage_state()
{
 LispPTR cons();

 if((*MACHINETYPE_word & 0xffff)== KATANA)
  {
	if(InterfacePage->dl24bitaddressable !=0)
		*STORAGEFULLSTATE_word = S_POSITIVE | SFS_SWITCHABLE ;
	else
		*STORAGEFULLSTATE_word = S_POSITIVE | SFS_NOTSWITCHABLE ;

	*SYSTEMCACHEVARS_word = cons(STORAGEFULLSTATE_index, *SYSTEMCACHEVARS_word);
	 GCLOOKUP(*SYSTEMCACHEVARS_word, ADDREF);
#ifdef DEBUG
	printf("SETSTATE: set to %d \n",(*STORAGEFULLSTATE_word)& 0xffff);
#endif
  }
 else
  {
	error("set_storage_state: Sorry! We can work on only KATANA");
  }

} /* set_storage_state() end */


LispPTR dremove(x,l)
LispPTR x,l;
{
 LispPTR z;

 if( Listp(l)==NIL) return(NIL);
 else if (x == car(l))
	  {
		if(cdr(l)!=NIL)
		 {
			rplaca(l,car(cdr(l)));
			rplacd(l,cdr(cdr(l)));
			return(dremove(x,l));
		 }
	  }
	 else
	  {
			z=l;
		lp:	if(Listp(cdr(l)) ==NIL)
				return(z);
			else if(x==car(cdr(l)))
				   rplacd(l , cdr(cdr(l)));
				else
				   l=cdr(l);
			goto lp;
	  }
}


/*****************************************************************/
/*
		Func Name :	newpage(addr)

		Created	:	Oct. 12, 1987 Takeshi Shimizu
		Changed :	Oct. 13, 1987 take
				OCt. 20, 1987 take

*/
/*****************************************************************/
LispPTR newpage(base)
  LispPTR base;
  {
    register DLword vp;
    register INTSTAT *int_state;

    extern DLword *FPtoVP;
    extern LispPTR *LASTVMEMFILEPAGE_word;
    extern LispPTR *VMEM_FULL_STATE_word;
    unsigned int nactive;
   vp=base >>8;

#ifdef DEBUG
    /************************/
    if(vp == 0) error ("newpage: vp=0");
    printf("***newpage modify vmemsize %d ",InterfacePage->nactivepages);
    /*************************/
#endif
    nactive = ++(InterfacePage->nactivepages);
    if ((nactive & 0xff) == 0)
      { /* need to add virtual page for fptovp first */
	unsigned int vp_of_fp, fp_of_fptovp;

	/* compute virtual page of new FPtoVP table page */
	vp_of_fp = (LADDR_from_68k(FPtoVP+nactive) >> 8);

	/* compute file page where this entry has to be to ensure
	   that FPtoVP is contiguous on the file */

	fp_of_fptovp = InterfacePage->fptovpstart +
		 (vp_of_fp - (LADDR_from_68k(FPtoVP) >> 8));

	/* debugging check: make sure FPtoVP is contiguous */

	if (GETWORD(FPtoVP + fp_of_fptovp - 1) != vp_of_fp -1 )
		{printf ("FPtoVP not contiguous\n");}

	/* move the file page for the previous VMEM page */
	GETWORD(FPtoVP + nactive) = GETWORD(FPtoVP + fp_of_fptovp);

	/* now, store virtual page of FPtoVP in FPtoVP table */

	GETWORD(FPtoVP + fp_of_fptovp) = (vp_of_fp);

	/* now we can make room for the new page we're adding */
	nactive = ++(InterfacePage->nactivepages);
      }
    GETWORD(FPtoVP + nactive) = vp ;

#ifdef DEBUG
    /*************************/
    printf("to %d  with VP:%d \n",InterfacePage->nactivepages,vp);
    /************************/
#endif

    if(InterfacePage->nactivepages >
	( ((*LASTVMEMFILEPAGE_word) & 0xffff) - GUARDVMEMFULL ) )
      {
	/* set vmemfull state */
	if(*VMEM_FULL_STATE_word ==NIL)
	  {
	    int_state=(INTSTAT *)Addr68k_from_LADDR(*INTERRUPTSTATE_word);
	    int_state->vmemfull=T;
	   *PENDINGINTERRUPT_word =ATOM_T;
	  }
	if(InterfacePage->nactivepages < ((*LASTVMEMFILEPAGE_word) & 0xffff))
	  {
	    *VMEM_FULL_STATE_word =S_POSITIVE ; /* set 0 */
	  }
	else if(InterfacePage->key == IFPVALID_KEY)
	  {
	    *VMEM_FULL_STATE_word = ATOM_T;
	  }
	else *VMEM_FULL_STATE_word = make_atom("DIRTY",0,5,0);
      }

    return(base);

  } /* newpage */

/** CHANGES FOR NEW STORAGE **/
/*****************************************************************/
/*
		Func Name :	init_storage()
		Description:
				Set values which are referenced by 
				Lisp Storage handling funcs 

		Created	:	Apr-23 1990 Takeshi Shimizu
		Changed :	
*/
/*****************************************************************/
init_storage()
{
/*LispPTR *ArraySpace2_word;*/

*SecondMDSPage_word = S_POSITIVE |( InterfacePage->dllastvmempage
		- PAGESPER_MDSUNIT -1);

} /* init_storage */
/** CHANGES FOR NEW SOTRAGE  END **/

