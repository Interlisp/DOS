/* @(#) miscn.c Version 1.14 (4/21/92). copyright envos & Fuji Xerox  */
static char *id = "@(#) miscn.c	1.14 4/21/92		(envos & Fuji Xerox)";


/************************************************************************/
/*									*/
/*	Copyright 1989, 1990 Venue, Fuji Xerox Co., Ltd, Xerox Corp.	*/
/*									*/
/*	This file is work-product resulting from the Xerox/Venue	*/
/*	Agreement dated 18-August-1989 for support of Medley.		*/
/*									*/
/************************************************************************/

/***********************************************************/
/*
		File Name:	miscn.c
		Including:	OP_miscn
*/
/***********************************************************/

#include <stdio.h>

#include "lispemul.h"
#include "address.h"
#include "adr68k.h"
#include "lsptypes.h"
#include "lispmap.h"
#include "emlglob.h"
#include "lspglob.h"
#include "arith.h"
#include "subrs.h"
#include "profile.h"

/***********************************************************/
/*
	Func Name  :	OP_miscn

	Interface:	Global Machine State
	Returns:	(must UFN)
			0 = continue, C code succeeded.
			1 = must UFN, C code failed.
*/
/***********************************************************/

int OP_miscn(misc_index, arg_count)
  int misc_index, arg_count;
{
  register LispPTR *stk;
  register int result;
  static LispPTR args[255];

/* Put the Args into a Vector */

	args[0] = NIL_PTR;
	stk = ((LispPTR *) CurrentStackPTR) + 1;

	{register int arg_num = arg_count;
	if (arg_num > 0)  {
		*stk++ = (LispPTR) TopOfStack;
		while(arg_num > 0) args[--arg_num] = *--stk;
		}
	}

/* Select the Misc Number */

switch(misc_index)
  {
      case miscn_USER_SUBR:
	{register LispPTR user_subr, user_args;
	 N_GETNUMBER(args[0], user_subr, do_ufn);
	 if ((result = UserSubr(user_subr, arg_count-1, &args[1])) < 0)
		goto do_ufn;
	}
	break;
      case miscn_SXHASH:  result = SX_hash(args);
			  break;

      case miscn_STRING_EQUAL_HASHBITS:
			  result = STRING_EQUAL_HASHBITS(args);
			  break;

      case miscn_STRINGHASHBITS:
			  result = STRING_HASHBITS(args);
			  break;

      case miscn_VALUES:
			if ( arg_count > 255 ) {
				error ( "miscn: arg_count too big! continue punts" );
				goto do_ufn;
				break;
				}
			  result = values(arg_count, args);
			  break;

      case miscn_VALUES_LIST:
			/*** debugging: should be impossible, but ADB found this once -FS *****/
			if ( arg_count > 255 ) {
				error ( "miscn: arg_count too big! continue punts" );
				goto do_ufn;
				break;
				}
			  result = values_list(arg_count, args);
			  break;

      case miscn_LCFetchMethod:
			  result = LCFetchMethod(args[0],args[1]);
			  if (result < 0) goto lc_ufn;
			  break;

      case miscn_LCFetchMethodOrHelp:
			  result = LCFetchMethodOrHelp(args[0],args[1]);
			  if (result < 0) goto lc_ufn;
			  break;

      case miscn_LCFindVarIndex:
			  result = LCFindVarIndex(args[0],args[1]);
			  if (result < 0) goto lc_ufn;
			  break;

      case miscn_LCGetIVValue:
			  result = LCGetIVValue(args[0],args[1]);
			  if (result < 0) goto lc_ufn;
			  break;

      case miscn_LCPutIVValue:
			  result = LCPutIVValue(args[0],args[1],args[2]);
			  if (result < 0) goto lc_ufn;
			  break;
#ifdef RS232
      case miscn_RAW_RS232C_OPEN:
			if((result = raw_rs232c_open(args[0]))==NIL)
				goto do_ufn;

			break;

      case miscn_RAW_RS232C_CLOSE:
			if((result = raw_rs232c_close(args[0]))==NIL)
				goto do_ufn;

			break;


      case miscn_RAW_RS232C_SETPARAM:
			if((result = raw_rs232c_setparams(args[0],args[1]))
						== NIL) goto do_ufn;
			break;
      case miscn_RAW_RS232C_GETPARAM:
			/******/ break;
      case miscn_RAW_RS232C_READ:
			if((result = raw_rs232c_read(args[0],args[1],args[2]))
				== NIL) goto do_ufn;
			break;
      case miscn_RAW_RS232C_WRITE:
			if((result = raw_rs232c_write(args[0],args[1],args[2]))
				== NIL) goto do_ufn;
			break;
      case miscn_RAW_RS232C_SETINT:
			if((result = raw_rs232c_setint(args[0],args[1]))
				==NIL) goto do_ufn;
			break;
      case miscn_CHATTER:
                        result = chatter(args);
                        break;
#endif /* RS232 */

#ifdef JLISP
      case miscn_EJLISP:
#ifndef NOWNN
                        result = ejlisp(args);
#endif
                        break;
#endif /* JLISP */

#ifdef CDROM
      case miscn_CDAUDIO:
                        result = cdaudio(args);
                        break;
      case miscn_CDROM:
                        result = cdrom(args);
                        break;
#endif /* CDROM */

      case /* miscn_CALL_C*/ 014:/* result = call_c_fn(args); */
			 break;

      default :
	goto do_ufn;
	break;

  }/* switch end */

/* Setup Global Machine State for a Normal Return */

	PC += 3;
	CurrentStackPTR = (DLword *) (stk - 1);
	TopOfStack = (LispPTR) result;
	return(0);

/* A UFN request, so return 1 & don't change the Machine State */

do_ufn:
	return(1);
lc_ufn:
	if (result == -2) {
	  return(0);		/* have built new stack frame */
	}else{
	  goto do_ufn;
	}


}/* OP_miscn */
