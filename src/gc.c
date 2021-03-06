/* @(#) gc.c Version 2.6 (4/21/92). copyright venue & Fuji Xerox  */
static char *id = "@(#) gc.c	2.6 4/21/92		(venue & Fuji Xerox)";

/*
 *	Copyright (C) 1987 by Fuji Xerox Co., Ltd. All rights reserved.
 *
 *	Author	:	Hiroshi Hayata	
 */
#include <stdio.h>
#include "lispemul.h"
#include "lspglob.h"
#include "lsptypes.h"
#include "emlglob.h"
#include "gc.h"

/************************************************************
 
	entry		OP_gcref		OPCODE[025]

	1. 	alpha is ADDREF or DELREF, STKREF.
		TopOfStack is argued slot address.
	2. 	call gclookup with alpha and TopOfStack.
	3.	if stk=0 and refcnt=0 of entry of HashMainTable, TopOfStack left alone.
		else replace TopOfStack with 0.
	4.	increment PC by 2.

***********************************************************/
 
OP_gcref()
{
#ifdef TRACE
	printPC();
	printf("TRACE:OP_gcref()\n");
#endif
	GCLOOKUPV(TopOfStack, Get_BYTE(PC+1), TopOfStack);
	PC+=2;
	return;
}	

