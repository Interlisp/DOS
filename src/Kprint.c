/* @(#) Kprint.c Version 2.12 (6/26/92). copyright venue & Fuji Xerox  */
static char *id = "@(#) Kprint.c	2.12 6/26/92	(venue & Fuji Xerox)";



/************************************************************************/
/*									*/
/*	(C) Copyright 1989, 1990, 1991 Venue. All Rights Reserved.	*/
/*	Manufactured in the United States of America.			*/
/*									*/
/*	The contents of this file are proprietary information 		*/
/*	belonging to Venue, and are provided to you under license.	*/
/*	They may not be further distributed or disclosed to third	*/
/*	parties without the specific permission of Venue.		*/
/*									*/
/************************************************************************/



/* 30 April 1987 */
/* print litatom,list,smallp */

/***********/

/* 9 Jun 1987 */
/* print string */
/* changed prindatum and added print_string */ 

/***********/

/* 2 July 1987 */
/* print fixp */
/* changed printdatum */
/************/
/* Aug 14 87 Take enable to print newstring(Lyric) */

/* 26 August 1987 */
/* changing for debug tool */
/* 6 May 1988 Modify for URAID by Take */


#include	<stdio.h>
#include	"print.h"
#include	"address.h"
#include	"lispemul.h"
#include	"lsptypes.h"
#include	"lspglob.h"
#include	"initatms.h"
#include	"cell.h"
#include	"emlglob.h"
#include	"lispmap.h"
#include	"adr68k.h"


int PrintMaxLevel= 2;
int Printdepth=0;


/************************************************************************/
/*									*/
/*			P R I N D A T U M				*/
/*									*/
/************************************************************************/

prindatum(x)
  LispPTR x;
 {
    NEWSTRINGP *newstring ;
    struct dtd *dtd_base;
    int	typen;
    DLword	typename;

    if(Printdepth > PrintMaxLevel)
      {
	if(GetTypeNumber(x)==TYPE_LISTP)
	  {
	    printf("(-)");
	  }
	  else printf("*");
	return;
      }
	

  switch (typen = GetTypeNumber(x))
   {
      case TYPE_LITATOM:
      case TYPE_NEWATOM:
			print_atomname(x);
			break;
      case TYPE_LISTP:
		Printdepth++;
		printf("%c" , LEFT_PAREN);		/* print "(" */
		lp: prindatum(car(x));
		    if (Listp (cdr(x)) == 0) 
		      {	/* print dotted pair */
			if ((x = cdr(x)) != NIL)
			  {
			    printf(" . ");
			    prindatum(x);
			  }
		      }
		     else
		      {
			printf("%c" , SPACE);
			x = cdr(x);
			goto lp;
		      }
		    printf("%c" , RIGHT_PAREN);	/* print ")" */
		    Printdepth--;
		    break;
	
	case	TYPE_SMALLP:
			if ((x & 0xff0000) == S_POSITIVE)
				printf("%d" , LOLOC(x));	/* print positive smallp */
				else						
				printf("%d" , (LOLOC(x) | 0xffff0000));	/* print negative smallp */
			break;
	case	TYPE_FIXP:
			print_fixp(x);		/* print fixp  */
			break;
	case TYPE_FLOATP:
			print_floatp(x);
			break;
	case TYPE_STRINGP:
			print_string(x);		/* print string */
			break;
	case TYPE_ONED_ARRAY :
	case TYPE_GENERAL_ARRAY :
			newstring=(NEWSTRINGP *)Addr68k_from_LADDR(x);
			if(newstring->stringp){
			  print_NEWstring(x);
			  break; }

	default: 
		dtd_base = (struct dtd *)GetDTD(typen);
		printf("{");
		if((typename = dtd_base->dtd_namelo+(dtd_base->dtd_namehi<<16)) != 0)
		  print_atomname(typename);
		else printf("unknown");
		printf("}0x");
		printf("%x" , x);	/* print lisp address in hex */
	}
}


/************************************************************************/
/*									*/
/*				P R I N T				*/
/*									*/
/*	Equivalent to the Lisp function PRINT,  prints the object.	*/
/*									*/
/************************************************************************/

LispPTR	print(x)
  LispPTR x;
  {
    Printdepth=0;
    prindatum(x&0xFFFFFF);
    /* printf("\n"); */		/* print CR */
    return (x);
  }



/************************************************************************/
/*									*/
/*			   p r i n t _ s t r i n g			*/
/*									*/
/*	Print a Lisp string.						*/
/*									*/
/************************************************************************/

print_string(x)
  LispPTR	x;
  {
    struct stringp	*string_point;
    DLword	st_length;
    DLbyte	*string_base;

    int		i;


    string_point = (struct stringp *)Addr68k_from_LADDR(x);
    st_length = string_point->length;
    string_base = (DLbyte *)Addr68k_from_LADDR(string_point->base);

    printf("%c" , DOUBLEQUOTE);	/* print %" */
		
    for (i = 1 ; i <= st_length ; i++)
      {
	printf("%c" , GETBYTE(string_base));
	string_base++;
      }

    printf("%c" , DOUBLEQUOTE);	/* print %" */
  }



/************************************************************************/
/*									*/
/*		      p r i n t _ N E W s t r i n g			*/
/*									*/
/*	Print a Lyric-style string (the Commonlisp-array kind), as	*/
/*	opposed to the older special-case STRINGP kind from Koto.	*/
/*									*/
/************************************************************************/

print_NEWstring(x)
  LispPTR	x;
  {
    NEWSTRINGP *string_point;
    DLword	st_length;
    DLbyte	*string_base;

    int		i;

    string_point = (NEWSTRINGP *)Addr68k_from_LADDR(x);
    st_length = string_point->fillpointer;
    string_base = (DLbyte *)Addr68k_from_LADDR(string_point->base);
    string_base += string_point->offset ;

    printf("%c" , DOUBLEQUOTE);	/* print %" */
		
    for (i = 0 ; i <= st_length ; i++)
      {
	printf("%c" , GETBYTE(string_base));
	string_base++;
      }

    printf("%c" , DOUBLEQUOTE);	/* print %" */
  }



/************************************************************************/
/*									*/
/*			p r i n t _ f i x p				*/
/*									*/
/*	Print a lisp integer (but not a bignum!)			*/
/*									*/
/************************************************************************/

print_fixp(x)
  LispPTR	x;
  {
    int	*addr_fixp;

    addr_fixp = (int *)Addr68k_from_LADDR(x);
    printf("%d" , *addr_fixp);
  }



/************************************************************************/
/*									*/
/*			p r i n t _ f l o a t p				*/
/*									*/
/*	Print a lisp floating-point value.				*/
/*									*/
/************************************************************************/

print_floatp(x)
  LispPTR	x;
  {
    float *addr_floatp;

    addr_floatp = (float *)Addr68k_from_LADDR(x);
    printf("%f" , *addr_floatp);
 }

	
