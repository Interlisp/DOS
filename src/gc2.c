/* This is G-file @(#) gc2.c Version 2.5 (4/21/92). copyright Xerox & Fuji Xerox  */
static char *id = "@(#) gc2.c	2.5 4/21/92";

/*
 *	Copyright (C) 1987 by Fuji Xerox Co., Ltd. All rights reserved.
 *
 *	Author	:	Naoyuki Mitani	
 */
/**********************************************************************/
/*
 		File Name :	gc2.c
		Desc	:	implement opcode SCAN1,SCAN2,GCRECLAIMCELL
 
 					Date :		Apr 13, 1987
 					Edited by :	Naoyuki Mitani
 
		Including :	OP_scan1
				OP_scan2
				OP_gcreccell
							
*/
/**********************************************************************/

#include "lispemul.h"
#include "lispmap.h"
#include "lsptypes.h"
#include "lspglob.h"
#include "emlglob.h"
#include "address.h"
#include "adr68k.h"


/**********************************************************************/
/*
		Func Name : OP_gcscan1
*/
/**********************************************************************/

OP_gcscan1()
{
DLword gcscan1();

#ifdef TRACE
	printPC();
	printf("TRACE: OP_gcscan1()\n");
#endif
	if((TopOfStack & 0xFF0000)==S_POSITIVE) {
		TopOfStack=gcscan1(LOLOC(TopOfStack));
	 }
	if(TopOfStack != NIL) TopOfStack |= S_POSITIVE ;
 PC++;
} /* OP_gcscan1 end */

/**********************************************************************/
/*
		Func Name : OP_gcscan2
*/
/**********************************************************************/

OP_gcscan2()
{
DLword gcscan2();

#ifdef TRACE
	printPC();
	printf("TRACE: OP_gcscan2()\n");
#endif
	if((TopOfStack & 0xFF0000)==S_POSITIVE) {
		TopOfStack=gcscan2(LOLOC(TopOfStack));
	 }
	if(TopOfStack != NIL) TopOfStack |= S_POSITIVE ;
 PC++;
} /* OP_gcscan2 end */

