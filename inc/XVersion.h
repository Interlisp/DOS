/* @(#) XVersion.h Version 1.3 (4/21/92). copyright Venue & Fuji Xerox  */



/************************************************************************/
/*									*/
/*	Copyright 1989, 1990 Venue, Fuji Xerox Co., Ltd, Xerox Corp.	*/
/*									*/
/*	This file is work-product resulting from the Xerox/Venue	*/
/*	Agreement dated 18-August-1989 for support of Medley.		*/
/*									*/
/************************************************************************/




#ifdef XWINDOW
#ifdef XV11R4
#undef XV11R1
#undef XV11R2
#undef XV11R3
#endif XV11R4

#ifdef XV11R3
#undef XV11R1
#undef XV11R2
#undef XV11R4
#endif XV11R3

#ifdef XV11R2
#undef XV11R1
#undef XV11R3
#undef XV11R4
#endif XV11R2

#ifdef XV11R1
#undef XV11R2
#undef XV11R3
#undef XV11R4
#endif XV11R1

#if ( !(defined( XV11R1 ))  \
   && !(defined( XV11R2 ))  \
   && !(defined( XV11R3 ))  \
   && !(defined( XV11R4 )) )
#define XV11R4			/* newest version */
#endif
#endif XWINDOW


