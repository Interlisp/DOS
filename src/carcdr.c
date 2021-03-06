/* @(#) car-cdr.c Version 2.10 (4/21/92). copyright Venue & Fuji Xerox  */
static char *id = "@(#) car-cdr.c	2.10 4/21/92		(envos & Fuji Xerox)";


/************************************************************************/
/*									*/
/*	Copyright 1989, 1990 Venue, Fuji Xerox Co., Ltd, Xerox Corp.	*/
/*									*/
/*	This file is work-product resulting from the Xerox/Venue	*/
/*	Agreement dated 18-August-1989 for support of Medley.		*/
/*									*/
/************************************************************************/

/*
 *	Author :  Takeshi Shimizu, Naoyuki Mitani
 */
/***********************************************************************/
/*
		File Name :	car-cdr.c

		Desc	:	car-cdr management

					Date :		Apr 24, 1987
					Edited by :	Naoyuki Mitani

		Including :	car
				cdr
				rplaca
				rplacd
				OP_car
				OP_cdr
				OP_rplaca
				OP_rplacd
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

short CarCdrError = 0;


/**********************************************************************/
/*
		Func name :	car

				Called from C program.

					Date :		Apr 15, 1987
					Edited by :	Naoyuki Mitani

*/
/**********************************************************************/

LispPTR car(datum)
	register LispPTR datum; /* datum must be LISP pointer(word offset) */
{
  register ConsCell *datum68k;
  register ConsCell *temp;

#ifdef TRACE2
	printf("TRACE: car()\n");
#endif

 datum68k = (ConsCell *)(Addr68k_from_LADDR(datum));
 if (Listp(datum))
 {
	if (datum68k->cdr_code == CDR_INDIRECT)
	{
		temp = (ConsCell *)Addr68k_from_LADDR(datum68k->car_field);
		return((LispPTR)temp->car_field);
	}
	else
		return((LispPTR)datum68k->car_field);
  }
 else if (datum==NIL_PTR)
	return((LispPTR)NIL_PTR);

 else {
	if(datum == ATOM_T)
	   return(ATOM_T);

	/** We assume CAR/CDRERR is CDR ***/
	else if ((datum & 0xFF0000)==0) /* LITATOM */
		return(NIL);
	else
		error("car : ARG not list");
      }
} /* end of car */

/**********************************************************************/
/*
	Func name :	cdr

		Called from C program.

			Date :		Apr 15, 1987
			Edited by :	Naoyuki Mitani

*/
/**********************************************************************/

LispPTR cdr(datum)
	register LispPTR datum; /* datum must be LISP pointer(word offset) */
{
 register ConsCell *datum68k;
 register DLword cdr_code;
 register  ConsCell *temp;

#ifdef TRACE2
	printf("TRACE: cdr()\n");
#endif

 datum68k = (ConsCell *)(Addr68k_from_LADDR(datum));
 cdr_code = datum68k->cdr_code;

 if (Listp(datum))
 {
	if (cdr_code == CDR_NIL)
		/* cdr-nil */

		return(NIL_PTR);

	else if ((cdr_code & CDR_ONPAGE) != 0)
		/* cdr-samepage */

		return(POINTER_PAGEBASE(datum) +
		((cdr_code & 127) << 1));

	else if (cdr_code == CDR_INDIRECT)	/* cdr_code > CDR_ONPAGE */
		/* cdr-indirect */

		return(cdr ((LispPTR)(datum68k->car_field)));

	else
	{
		/* cdr-differentpage */

		temp = (ConsCell *)(Addr68k_from_LADDR
		(POINTER_PAGEBASE(datum) + (cdr_code << 1)));
		return((LispPTR)temp->car_field);
	}
  }
 else if (datum==NIL_PTR) return(NIL_PTR);

 /**** We assume CAR/CDRERR is CDR ****************/
 else error("cdr : ARG not list");

  } /* end of cdr */



/**********************************************************************/
/*
		Func name :	rplaca

				Called from C program.

					Date :		Apr 15, 1987
					Edited by :	Naoyuki Mitani
*/
/**********************************************************************/

LispPTR rplaca(x , y)
	register LispPTR x;		/* car of x will be smashed */
	register LispPTR y;		/* y is a newly car object */
  {
    register ConsCell *x_68k;
    register ConsCell *temp;

#ifdef TRACE2
    printf("TRACE: rplaca()\n");
#endif

    if (Listp(x) == NIL)
      { /* arg isn't a CONS cell, might be NIL */
	if (x == NIL_PTR)
	  {
	    if (y != NIL_PTR) error("Attempt to RPLACA NIL");
	    else return(NIL_PTR);
	  }
	else error("ARG not List");
      }

    else
      {
	x_68k = (ConsCell *)Addr68k_from_LADDR(x);

	GCLOOKUP(car(x), DELREF);	/* set up reference count */
	GCLOOKUP(y, ADDREF);

	if (x_68k->cdr_code == CDR_INDIRECT)
	  {
	    temp = (ConsCell *)Addr68k_from_LADDR((LispPTR)x_68k->car_field);
	    temp->car_field = y;
	  }
	else x_68k->car_field = y;

	return(x);
      }
  } /* end of rplaca */




/**********************************************************************/
/*
		Func name :	rplacd

				Called from C program.

					Date :		Apr 16, 1987
					Edited by :	Naoyuki Mitani
*/
/**********************************************************************/

extern struct conspage *next_conspage();  /* conspage.c */

LispPTR rplacd(x , y)
	LispPTR x;		/* cdr of x will be smashed */
	register LispPTR y;		/* y is a newly cdr object */
  {
    register ConsCell *x_68k;
    register ConsCell *temp68k;
    register ConsCell *cdr_cell68k;
    LispPTR cdr_cell;
    LispPTR rp_page;
    DLword cdr_code;
    register struct conspage *cons68k;

#ifdef TRACE2
    printf("TRACE: rplacd()\n");
#endif

    if (Listp(x) == NIL)
      {
	if (x == NIL_PTR)
	  {
	    if (y != NIL_PTR) error("Attempt to RPLACD NIL");
	    else return(NIL_PTR);
	  }
	else error("ARG not List");
      }

    else
      {
	x_68k = (ConsCell *)Addr68k_from_LADDR(x);

	GCLOOKUP(cdr(x), DELREF);	/* set up reference count */
	GCLOOKUP(y, ADDREF);

	cdr_code = x_68k->cdr_code;

	if (cdr_code == CDR_INDIRECT)
	  {
	    /* cdr-indirect */

	    rp_page = (LispPTR)x_68k->car_field;
	    temp68k = (ConsCell *)Addr68k_from_LADDR(rp_page);
	    cdr_cell = POINTER_PAGEBASE(rp_page) +
				(temp68k->cdr_code << 1);
#ifdef DEBUG
/**** Chechk for DEBUG ***/
if(temp68k->cdr_code >= 0200 ||  temp68k->cdr_code == 0){
		printf("temp68k->cdr_code: %x\n", temp68k->cdr_code);
}
/**** End Check for DEBUG ***/
#endif

	    cdr_cell68k = (ConsCell *)Addr68k_from_LADDR(cdr_cell);
	    *(LispPTR *)cdr_cell68k = y & 0xffffff; /* cdr_code is set to 0 */
	  }
	else if (cdr_code <= CDR_MAXINDIRECT)
	  {
	    /* cdr-differentpage */

	    cdr_cell = POINTER_PAGEBASE(x) + (cdr_code << 1);
	    cdr_cell68k = (ConsCell *)Addr68k_from_LADDR(cdr_cell);
	    *(LispPTR *)cdr_cell68k = y & 0xffffff; /* cdr_code is set to 0 */

	  }
	else if (y == NIL_PTR)
		/* cdr-samepage & y is nil */
		x_68k->cdr_code = CDR_NIL;

	else if ((rp_page = POINTER_PAGEBASE(x)) == POINTER_PAGEBASE(y))
		/* cdr-samepage & x and y are on same page */
		x_68k->cdr_code = CDR_ONPAGE + ((y & 0xff) >> 1);

	else
	  {
	    /* cdr-samepage & x and y are on different page */

	    cons68k = (struct conspage *)(Addr68k_from_LADDR(rp_page));
	    if (cons68k->count > 0)
	      {
		/* at least one free-cell on x's conspage */
		cdr_cell68k = GetNewCell_68k(cons68k);
		cons68k->count--;
		cons68k->next_cell = cdr_cell68k->cdr_code;

		*(LispPTR *)cdr_cell68k = y & 0xffffff; /* cdr_code set to 0 */

		x_68k->cdr_code = (LADDR_from_68k(cdr_cell68k) -
		rp_page) >> 1;
	      }
	    else
	      {
		/* no more free-cell on x's conspage */

		cons68k = next_conspage();

		cdr_cell68k = GetNewCell_68k(cons68k);
		cons68k->next_cell = cdr_cell68k->cdr_code;
		temp68k = GetNewCell_68k(cons68k);
		cons68k->next_cell = temp68k->cdr_code;

		cons68k->count -= 2;

		*(LispPTR *)cdr_cell68k = y & 0xffffff; /* cdr_code set to 0 */

		temp68k->car_field = x_68k->car_field;
		x_68k->car_field = LADDR_from_68k(temp68k);

		temp68k->cdr_code = (LADDR_from_68k(cdr_cell68k) & 0xff) >> 1;

		x_68k->cdr_code = CDR_INDIRECT;
	      }
	  }
      }
    return(x);

  }/* end of rplacd */




/**********************************************************************/
/*
		Func name :	N_OP_car

				car management

					Date :		March 21, 1988
					Edited by :	Robert Krivacic

*/
/**********************************************************************/

LispPTR N_OP_car(tos)
	register LispPTR tos;
  {
    register ConsCell *datum68k;
    register ConsCell *temp;
 
    datum68k = (ConsCell *)(Addr68k_from_LADDR(tos));
    if (Listp(tos))
      {
	if (datum68k->cdr_code == CDR_INDIRECT)
	  {
	    temp = (ConsCell *)Addr68k_from_LADDR(datum68k->car_field);
	    return((LispPTR) temp->car_field);
	  }
	else return((LispPTR) datum68k->car_field);
       }
    else if (tos == NIL_PTR) return(tos);
    else if ( tos == ATOM_T) return(tos);
    else
      {
	ERROR_EXIT(tos);
      }
  } /* end of N_OP_car */




/**********************************************************************/
/*
		Func name :	N_OP_cdr

				cdr management

					Date :		March 21, 1988
					Edited by :	Robert Krivacic
*/
/**********************************************************************/

LispPTR N_OP_cdr(tos)
	register LispPTR tos;
{
    register ConsCell *datum68k;
    register DLword cdr_code;
    register ConsCell *temp;

    datum68k = (ConsCell *)(Addr68k_from_LADDR(tos));
    cdr_code = datum68k->cdr_code;

    if (Listp(tos))
      {
	if (cdr_code == CDR_NIL) return(NIL_PTR);	/* cdr-nil */

	else if (cdr_code > CDR_ONPAGE)		/* cdr-samepage */
		return(POINTER_PAGEBASE(tos) + ((cdr_code & 127) << 1));

	else if (cdr_code == CDR_INDIRECT)	/* cdr_code < CDR_ONPAGE */
						/* cdr-indirect */
		return(cdr ((LispPTR)(datum68k->car_field)));

	else
	  {
	    /* cdr-differentpage */

	    return((LispPTR)
			((ConsCell *)
			(Addr68k_from_LADDR
			(POINTER_PAGEBASE(tos)+(cdr_code << 1))))->car_field);
	  }
      }
    else if (tos == NIL_PTR) return(tos);
    else
      {
	ERROR_EXIT(tos);
      }


  } /* end of N_OP_cdr */




/**********************************************************************/
/*

		Func name :	N_OP_rplaca

				rplaca management

					Date :		March 21, 1988
					Edited by :	Robert Krivacic
*/
/**********************************************************************/

LispPTR N_OP_rplaca(tosm1, tos)
	register LispPTR tosm1;
	register LispPTR tos;
{
register  ConsCell *x_68k;
register ConsCell *temp;

 if (Listp(tosm1) == NIL)
 {
	if (tosm1 == NIL_PTR)
	{
		if (tos != NIL_PTR) ERROR_EXIT(tos)
		else
			return(tosm1);
	}
	else
		ERROR_EXIT(tos);
 }

 else
 {
	x_68k = (ConsCell *)Addr68k_from_LADDR(tosm1);

	GCLOOKUP(car(tosm1), DELREF);	/* set up reference count */
	GCLOOKUP(tos, ADDREF);

	if (x_68k->cdr_code == CDR_INDIRECT)
	{
		temp = (ConsCell *)Addr68k_from_LADDR
					((LispPTR)x_68k->car_field);
		temp->car_field = tos;
	}
	else
		x_68k->car_field = tos ;

	return(tosm1);
 }
} /* end of N_OP_rplaca */




/**********************************************************************/
/*
		Func name :	N_OP_rplacd

				rplacd management

					Date :		March 21, 1988
					Edited by :	Robert Krivacic


*/
/**********************************************************************/

LispPTR N_OP_rplacd(tosm1, tos)
	register LispPTR tosm1, tos;
{
 register ConsCell *x_68k;
 register ConsCell *temp68k;
 register ConsCell *cdr_cell68k;
 LispPTR cdr_cell;
 LispPTR rp_page;
 DLword cdr_code;
 register struct conspage *cons68k;

 if (Listp(tosm1) == NIL)
 {
	if (tosm1 == NIL_PTR)
	{
		if (tos != NIL_PTR)
			ERROR_EXIT(tos)
		else
			return(tosm1);
	}
	else
		ERROR_EXIT(tos);
 }

 else
 {
	x_68k = (ConsCell *)Addr68k_from_LADDR(tosm1);

	GCLOOKUP(cdr(tosm1), DELREF);	/* set up reference count */
	GCLOOKUP(tos, ADDREF);

	cdr_code = x_68k->cdr_code;

	if (cdr_code == CDR_INDIRECT)
	{
		/* cdr-indirect */

		rp_page = (LispPTR)x_68k->car_field;
		temp68k = (ConsCell *)Addr68k_from_LADDR(rp_page);
		cdr_cell = POINTER_PAGEBASE(rp_page) +
				(temp68k->cdr_code << 1);
		cdr_cell68k = (ConsCell *)Addr68k_from_LADDR(cdr_cell);
#ifdef DEBUG
/**** Chechk for DEBUG ***/
if(temp68k->cdr_code >= 0200 ||  temp68k->cdr_code == 0){
		printf("temp68k->cdr_code: %x\n", temp68k->cdr_code);
}
/**** End Chechk for DEBUG ***/
#endif
		*(LispPTR *)cdr_cell68k = tos & 0xffffff;
			/* cdr_code is set to 0 */

	}
	else if (cdr_code <= CDR_MAXINDIRECT)
	{
		/* cdr-differentpage */

		cdr_cell = POINTER_PAGEBASE(tosm1) + (cdr_code << 1);
		cdr_cell68k = (ConsCell *)Addr68k_from_LADDR(cdr_cell);
		*(LispPTR *)cdr_cell68k = tos & 0xffffff;
			/* cdr_code is set to 0 */

	}
	else if (tos == NIL_PTR)
		/* cdr-samepage & tos is nil */
		x_68k->cdr_code = CDR_NIL;

	else if ((rp_page = POINTER_PAGEBASE(tosm1)) ==
					POINTER_PAGEBASE(tos))
		/* cdr-samepage & tosm1 and tos are on same page */
		x_68k->cdr_code = CDR_ONPAGE + ((tos & 0xff) >> 1);

	else
	{
		/* cdr-samepage & tosm1 and tos are on different page */

		cons68k = (struct conspage *)Addr68k_from_LADDR(rp_page);
		if (cons68k->count > 0)
		{
			/* at least one free-cell on tosm1's conspage */
			cdr_cell68k = GetNewCell_68k(cons68k);
			cons68k->count--;
			cons68k->next_cell = cdr_cell68k->cdr_code;

			*(LispPTR *)cdr_cell68k = tos & 0xffffff;
				/* cdr_code is set to 0 */

			x_68k->cdr_code = (LADDR_from_68k(cdr_cell68k) -
			rp_page) >> 1;
		}
		else
		{
			/* no more free-cell on x's conspage */

			cons68k = next_conspage();

			cdr_cell68k = GetNewCell_68k(cons68k);
			cons68k->next_cell = cdr_cell68k->cdr_code;

			temp68k = GetNewCell_68k(cons68k);
			cons68k->next_cell = temp68k->cdr_code ;
			cons68k->count -= 2;

			*(LispPTR *)cdr_cell68k = tos & 0xffffff;
				/* cdr_code is set to 0 */

			temp68k->car_field = x_68k->car_field;
			x_68k->car_field = LADDR_from_68k(temp68k);

			temp68k->cdr_code = (LADDR_from_68k(cdr_cell68k)
			& 0xff) >> 1;

			x_68k->cdr_code = CDR_INDIRECT;
		}
	}
 }
 return(tosm1);

}/* end of N_OP_rplacd */



