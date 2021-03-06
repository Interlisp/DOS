/* @(#) conspage.c Version 2.11 (4/21/92). copyright venue & Fuji Xerox  */
static char *id = "@(#) conspage.c	2.11 4/21/92		(venue & Fuji Xerox)";

/************************************************************************/
/*									*/
/*	Copyright 1989, 1990 Venue, Fuji Xerox Co., Ltd, Xerox Corp.	*/
/*									*/
/*	This file is work-product resulting from the Xerox/Venue	*/
/*	Agreement dated 18-August-1989 for support of Medley.		*/
/*									*/
/************************************************************************/

/***********************************************************************/
/*
		File Name :conspage.c
*/
/************************************************************************/
#include "lispemul.h"
#include "address.h"
#include "adr68k.h"
#include "lsptypes.h"
#include "cell.h"
#include "lispmap.h"
#include "lspglob.h"
#include "gc.h"

/**********************************************************************/
/*
		Func name :	next_conspage

		GET NEXT CONS PAGE .

		Date :		January 13, 1987
		Edited by :	Takeshi Shimizu
		Changed	:	January 20, 1987 (take)
		Changed :	Feb-12-87 take
		Changed :	Feb-13-87 take

*/
/**********************************************************************/

struct conspage *next_conspage()
  {
    extern struct dtd *ListpDTD ;

    register struct  conspage  *page1 ;  /* Allocated 1st MDS page */
    register struct  conspage  *page2 ;  /* Allocated 2nd MDS page */

    register int next;

#ifdef TRACE2
    printf("TRACE: next_conspage()\n");
#endif

    for(next =(int) ListpDTD->dtd_nextpage ; /* getnext free conspage */ 
        ; ListpDTD -> dtd_nextpage = next = page1 -> next_page ,
          page1->next_page = 0xffff )
      {
	if(next ==0)
	  {
	    /* Alloc 2 Conspages and get 1st page base */
	    page1 = (struct conspage *)alloc_mdspage(TYPE_LISTP) ;

	    /* Culc. next Conspage's Base address */
	    page2 = (struct conspage *)((DLword *)page1 + DLWORDSPER_PAGE) ;
#ifdef TRACE2
	    printf("DEB: nextcons : new cons page %x and %x \n",
			LADDR_from_68k(page1),LADDR_from_68k(page2));
#endif

	    init_conspage(page2 , NIL_PTR); /* Doesn't exst next page */
	    init_conspage(page1 , LPAGE_from_68k(page2) ) ;

	    ListpDTD->dtd_nextpage = LPAGE_from_68k(page1) ;
	    goto ex ; /* replaced break */
	  }
	else
	  {
	    page1 =(struct conspage *) Addr68k_from_LPAGE(next) ; /*Jan-21*/
	  }

	if (page1->count > 1) break ;

      } /* for loop end */
ex :
    return(page1) ;
  }  /* next_conspage end */




/**********************************************************************/
/*
	Func name :	init_conspage

		GET NEXT CONS PAGE .

			Date :		January 13, 1987
			Edited by :	Takeshi Shimizu
			Changed :	Feb-13-87 take
					Nov-13-87 take(DLword ->int)

*/
/**********************************************************************/

init_conspage( base , link )
  register struct conspage *base;  /* Page Base */
  DLword link ; /* Prev Link page number DL->int*/
  {
    register ConsCell *cell ;
    register int j ; /* DL-> int */

#ifdef TRACE2
    printf("TRACE: init_conspage()\n");
#endif

    base -> next_cell= j = 254 ;
  
    while(j != 0)
     {
	cell = (ConsCell *)((DLword *)base + j );
	cell->car_field = NIL_PTR ;
	cell->cdr_code = j = j-2 ;
     }
    base -> count = 127 ;
    base -> next_page = link ;

  } /* init_conspage end */




/**********************************************************************/
/* function cons same as N_OP_cons				      */
/**********************************************************************/

cons(cons_car , cons_cdr)
  LispPTR  cons_car, cons_cdr ;
  {
    return(N_OP_cons(cons_car, cons_cdr));
  }
  


/**********************************************************************/
/*
	Func name :N_OP_cons	
		Execute CONS OPCODE

			Date :		March 29 1988
			Edited by :	Bob Krivacic

*/
/**********************************************************************/

N_OP_cons(cons_car, cons_cdr)
  register int cons_car, cons_cdr;
  {
    extern struct dtd *ListpDTD ;

    register struct conspage *new_conspage ;
    register ConsCell *new_cell ;
    register ConsCell *temp_cell ;
    register int new_page ;  /* hold the return  val of nextconspage ,DL->int */


    GCLOOKUP(cons_cdr, ADDREF);
    GCLOOKUP(cons_car, ADDREF);

/*    IncAllocCnt(1); */

    if(cons_cdr == NIL_PTR)
      {
	if ((ListpDTD->dtd_nextpage != 0)&&(GetCONSCount(ListpDTD->dtd_nextpage) > 0))
	  {	/* nextpage has 1 or more free cells  in that page */
	    /* uCode  CDR_NIL case */
	    new_page = ListpDTD->dtd_nextpage;
	    /* Get New Cell */
	    new_conspage = (struct conspage *)Addr68k_from_LPAGE(new_page);
	    new_cell = GetNewCell_68k(new_conspage); /* get new cell */
#ifdef DEBUG
	    if(new_cell->car_field != NIL)
	      {
		printf("CELL 0x%x has non-NIL car = 0x%x \n",
		       LADDR_from_68k(new_cell), new_cell->car_field);
		error("QUIT from N_OP_cons");
	      }
#endif

	    new_conspage->count --;  /* decrement free cnt. */
	    new_conspage->next_cell = new_cell->cdr_code ; /* update free cell chain */

	    /* filling new cell with the data */
	    new_cell->car_field = cons_car ;
	    new_cell->cdr_code = CDR_NIL ;

	    ListpDTD->dtd_cnt0++;

	  } /* if (ListpDTD.. end */
	else /* 1   UFN case : CDR_NIL */
	  {
	    new_conspage=next_conspage();

	    new_cell = GetNewCell_68k( new_conspage ) ;
#ifdef DEBUG
	    if(new_cell->car_field != NIL)
	      {
		printf("CELL 0x%x has non-NIL car = 0x%x \n",
		       LADDR_from_68k(new_cell), new_cell->car_field);
		error("QUIT from N_OP_cons");
	      }
#endif

	    new_conspage->count --;  /* decrement free cnt. */
	    new_conspage->next_cell = new_cell->cdr_code ; /* update free cell chain */

	    /* filling new cell with the data */
	    new_cell->car_field = cons_car ;
	    new_cell->cdr_code = CDR_NIL ;

	    ListpDTD->dtd_oldcnt++;

	  } /* else 1 end */
      } /* if(cons_cdr.. end */
    else /* cons_cdr != NIL */
      {
	new_page = POINTER_PAGE(cons_cdr); /* Y's page num */
	if(Listp(cons_cdr) && (GetCONSCount(new_page ) > 0 ))
	  {
	    /* uCode case : CDR_ONPAGE */
	    new_conspage= (struct conspage *)Addr68k_from_LPAGE(new_page);

	    new_cell = GetNewCell_68k( new_conspage ) ;
#ifdef DEBUG
	    if(new_cell->car_field != NIL)
	      {
		printf("CELL 0x%x has non-NIL car = 0x%x \n",
		       LADDR_from_68k(new_cell), new_cell->car_field);
		error("QUIT from N_OP_cons");
	      }
#endif

	    new_conspage->count --;  /* decrement free cnt. */
	    new_conspage->next_cell = new_cell->cdr_code ; /* update free cell chain */

	    new_cell->car_field = cons_car ;
	    /* cdr_onpage + cell offset in this conspage */
	    new_cell->cdr_code = CDR_ONPAGE | ((cons_cdr&0xff)>>1) ;

	    ListpDTD->dtd_cnt0++;

		
	  } /* if (listp.. end */
	else
	  {
	    /* UFN case : CDR_INDIRECT */
	    new_conspage = next_conspage();

	    /* get 2 cells from conspage */
	    temp_cell = GetNewCell_68k( new_conspage ) ;
#ifdef DEBUG
	if(temp_cell->car_field != NIL) {
		printf("CDR indirect CELL 0x%x has non-NIL car 0x%x \n",
		       LADDR_from_68k(new_cell), temp_cell->car_field);
		error("QUIT from N_OP_cons");
	}
#endif

	    new_conspage->next_cell = temp_cell->cdr_code ; /* update free cell chain */
	    new_cell = GetNewCell_68k( new_conspage ) ;
#ifdef DEBUG
	if(new_cell->car_field != NIL) {
		printf("CDR ind-2 CELL 0x%x has non-NIL car = 0x%x \n",
		       LADDR_from_68k(new_cell), new_cell->car_field);
		error("QUIT from N_OP_cons");
	}
#endif

	    new_conspage->next_cell = new_cell->cdr_code ; /* update free cell chain */
	    new_conspage->count -= 2;  /* decrement free cnt. */

	    /* filling cell */
	    *((LispPTR *)temp_cell) = cons_cdr ; /* Indirect CDR ptr */

	    new_cell->car_field = cons_car ;
	    /* culc. cdr code */
	    new_cell->cdr_code = (((LispPTR)LADDR_from_68k(temp_cell)) & 0xff) >> 1;

	    ListpDTD->dtd_oldcnt++ ;  /* added feb-12 take */


	  } /* else end */

      } /* else (cons_cdr==NIL end) */

    /* return val set */
 
    new_page = LADDR_from_68k(new_cell) ;

    GCLOOKUP(new_page, DELREF);

    return(new_page);

  } /* N_OP_cons() end */
