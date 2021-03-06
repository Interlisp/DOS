/* @(#) ubf3.c Version 1.9 (4/22/92). copyright Venue & Fuji Xerox  */
static char *id = "@(#) ubf3.c	1.9 4/22/92";
/*	ubf3.c
 */


/************************************************************************/
/*									*/
/*	(C) Copyright 1989-92 Venue. All Rights Reserved.		*/
/*	Manufactured in the United States of America.			*/
/*									*/
/*	The contents of this file are proprietary information 		*/
/*	belonging to Venue, and are provided to you under license.	*/
/*	They may not be further distributed or disclosed to third	*/
/*	parties without the specific permission of Venue.		*/
/*									*/
/************************************************************************/



#include <stdio.h>
#include "lispemul.h"
#include "lspglob.h"
#include "adr68k.h"
#include "lispmap.h"
#include "fp.h"

/************************************************************
	N_OP_ubfloat3  -- op 062
062/0	POLY
***********************************************************/

N_OP_ubfloat3(arg3, arg2, arg1, alpha)
  int arg3;
  int alpha;
  LispPTR arg1, arg2;
  {
    REGISTER float val;
    REGISTER float ans;
    REGISTER float *fptr;
    register int degree;
    int ret;
    float flot;

    val = *(float *)&arg3;		/* why? */
    if (alpha) ERROR_EXIT(arg1);
    FPCLEAR;
    if ((arg1 & 0xFF0000) != S_POSITIVE) ERROR_EXIT(arg1);
    degree = 0xFFFF & arg1;
    fptr = (float *)Addr68k_from_LADDR(arg2);
    ans = *((float *)fptr);
    while (degree--) ans = (ans * val) + *((float *)(++fptr));
    if (FPTEST(ans)) ERROR_EXIT(arg1);	/* relies on contagion of inf, nan? */
    flot = ans;
    ret = *(int *)&flot;			/* why? */
    return(ret);
  }/* end N_OP_ubfloat3() */

	/* end module */
