/* @(#) address.h Version 2.5 (4/21/92). copyright envos & Fuji Xerox  */


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
 		File Name :	address.h

		Address Manipulate Macros(for LISP pointer)

 					Date :		December 8, 1986
 					Edited by :	Takeshi Shimizu
 					Changed :	Dec.22.86 (take)
					Changed :	Jan.14.87(take)
					Changed :	Apr.20.87(mitani)
							Sep.02.87 take
							(add parenthesises)
*/
/**********************************************************************/

/* NOTE: Thease MACRO shuld be used for the pointers in LISP SYSOUT */
#define LLSH(datum , n)		((datum )<< n)
#define LRSH(datum , n)		((datum) >> n)

#define HILOC(ptr)			(LRSH(((int)(ptr) & 0x0ff0000),16))
#define LOLOC(ptr)			((int)(ptr) & 0x0ffff)

#define VAG2(hi,lo)			(LLSH((hi),16) | (lo))


/* NOTE: argument off must be WORD offset */
#define ADDBASE(ptr,off)		((int)(ptr) + (off))
#define GETBASE(ptr,off)		(GETWORD(((DLword *)(ptr)) + (off)))



/* Following MACRO defs. is related with POINTER which is defined as ACCESSFNS in Interlisp(LLNEW)  */

#define POINTER_PAGE(datum)	(((int)(datum) & 0x0ffff00) >> 8 )
#define POINTER_SEGMENT(datum)		HILOC(datum)
#define POINTER_PAGEBASE(datum)	((datum) & 0x0ffff00)

