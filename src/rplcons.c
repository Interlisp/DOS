/* @(#) rplcons.c Version 2.9 (4/21/92). copyright venue & Fuji Xerox  */
static char *id = "@(#) rplcons.c	2.9 4/21/92		(venue & Fuji Xerox)";

/*
 *	Copyright (C) 1987 by Fuji Xerox Co., Ltd. All rights reserved.
 *
 *	Author :  Takeshi Shimizu
 *
 */
/***********************************************************************/
/*
 		File Name :	rplcons.c

		Desc	:	rplcons
 
 				Date :		Jun. 1, 1987
 				Edited by :	Takeshi Shimizu
				Changed :	Jun. 2 1987 take
				Changed :	Jun. 12 1987 take
				Changed :	Oct. 22 1987 take
 
		Including :	rplcons
				OP_rplcons
				
*/
/**********************************************************************/

#include "lispemul.h"
#include "emlglob.h"
#include "lspglob.h"
#include "lsptypes.h"
#include "address.h"
#include "adr68k.h"
#include "gc.h"
#include "cell.h"    

/***************************************************/

N_OP_rplcons(list, item)
	register LispPTR list;
	register LispPTR item;
{
 register struct conspage *conspage ;
 register ConsCell *new_cell , *list68k;

 LispPTR register page;

 if(!Listp(list)) ERROR_EXIT(item);

 page = POINTER_PAGE(list);
 list68k=(ConsCell *)Addr68k_from_LADDR(list);

/* There are some rest Cell and "list" must be ONPAGE cdr_coded */

 if((GetCONSCount(page) != 0) && (list68k->cdr_code > CDR_MAXINDIRECT))
  {
	GCLOOKUP(item, ADDREF);
	GCLOOKUP(cdr(list), DELREF);

	conspage = (struct conspage *)Addr68k_from_LPAGE(page);
	new_cell =(ConsCell *)GetNewCell_68k(conspage);

	conspage->count--;
	conspage->next_cell= new_cell->cdr_code;

	new_cell->car_field = item ;
	new_cell->cdr_code = CDR_NIL ;

	ListpDTD->dtd_cnt0++;

	list68k->cdr_code = CDR_ONPAGE | ((LADDR_from_68k(new_cell) &0xff)>>1) ;

	return(LADDR_from_68k(new_cell)) ; 


  }
 else
  {
	N_OP_rplacd(list, item = cons(item , NIL_PTR), 0);
	return(item);
  }
}

