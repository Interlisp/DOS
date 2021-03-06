/* @(#) mvs.c Version 1.1 (5/15/89). copyright Venue & Fuji Xerox  */
static char *id = "@(#) mvs.c	1.1 5/15/89	(Venue & Fuji Xerox)";



/************************************************************************/
/*									*/
/*	Copyright 1989, 1990 Venue.					*/
/*									*/
/*	This file was work-product resulting from the Xerox/Venue	*/
/*	Agreement dated 18-August-1989 for support of Medley.		*/
/*									*/
/************************************************************************/


/************************************************************************/
/*									*/
/*		M U L T I P L E - V A L U E   S U P P O R T		*/
/*			    F U N C T I O N S				*/
/*									*/
/*	Contains: values, values_list					*/
/*									*/
/************************************************************************/

#include <stdio.h>


#include "lispemul.h"
#include "lispmap.h"
#include "lspglob.h"
#include "emlglob.h"
#include "adr68k.h"
#include "lsptypes.h"
#include "stack.h"
#include "opcodes.h"

#ifdef AIXPS2
#include "inlnPS2.h"
#endif /* AIXPS2 */



LispPTR MVLIST_index;


/****************************************************************/
/*                                                              */
/*                            VALUES                            */
/*								*/
/*         C-coded version of the function CL:VALUES		*/
/*								*/
/****************************************************************/

LispPTR values (arg_count, args)
  register LispPTR *args;
  int arg_count;
  {
    FX2 *caller, *prevcaller=0, *immediate_caller=0;
    ByteCode *pc;
    int unbind_count=0;
    struct fnhead *fnhead;
    short opcode;

    caller = (FX2 *) CURRENTFX;
    immediate_caller = caller;

newframe:
    if (caller == immediate_caller)
      {
	fnhead = (struct fnhead *) FuncObj;
	pc = (ByteCode *) PC+3; /* to skip the miscn opcode we're in now */
      }
    else
      {
	fnhead = (struct fnhead *)
		    Addr68k_from_LADDR(0xFFFFFF & swapx((int)caller->fnheader));
	pc = (ByteCode *)fnhead+(caller->pc);
      }
newpc:
    opcode = (short)((unsigned char)GETBYTE((char *)pc));
    switch (opcode)
      {
	case opc_RETURN:
	case opc_SLRETURN:  prevcaller = caller;
			    caller = (FX2 *) (Stackspace+(int)(GETCLINK(caller)));
			    goto newframe;

	case opc_FN1:   if (MVLIST_index == Get_AtomNo(pc+1))
			 {
			   if (unbind_count > 0)
			     simulate_unbind(caller, unbind_count, prevcaller);
#ifndef BIGATOMS
			    /* would add 3 to  PC, but miscn return code does.*/
			   if (caller == immediate_caller) PC = pc;
#else
			   /* BUT 3's not enough for big atoms, so add 1 */
			   if (caller == immediate_caller) PC = pc + 1;
#endif /* BIGATOMS */

			   else caller->pc = (int)pc+ FN_OPCODE_SIZE-(int)fnhead;
			   return(make_value_list(arg_count, args));
			 }
			break;

	case opc_UNBIND:  pc += 1;
			  unbind_count += 1;
			  goto newpc;

	case opc_JUMPX:  {
			   register short displacement;
			   displacement = (short) (GETBYTE((char *)pc+1));
			   if (displacement >= 128) displacement -= 256;
			   pc += displacement;
			   goto newpc;
			 }

	case opc_JUMPXX:  {
			    register int displacement;
			    displacement = (int) Get_DLword(pc+1);
			    if (displacement >= 32768) displacement -= 65536;
			    pc += displacement;
			    goto newpc;
			  }
	default:  if ((opcode >= opc_JUMP) && (opcode < opc_FJUMP))
		    {
		      pc += 2 + opcode - opc_JUMP;
		      goto newpc;
		    }
      }

	/*****************************************/
	/* Default case:  Return a single value. */
	/*****************************************/

    if (arg_count>0) return(args[0]);
    else return(NIL_PTR);
  }




/****************************************************************/
/*                                                              */
/*                            VALUES_LIST         		*/
/*								*/
/*         C-coded version of the function CL:VALUES-LIST 	*/
/*								*/
/****************************************************************/

LispPTR values_list (arg_count, args)
  register LispPTR *args;
  int arg_count;
  {
    FX2 *caller, *prevcaller=0, *immediate_caller=0;
    ByteCode *pc;
    int unbind_count=0;
    struct fnhead *fnhead;
    short opcode;

    caller = (FX2 *) CURRENTFX;
    immediate_caller = caller;

newframe:
    if (caller == immediate_caller)
      {
	fnhead = (struct fnhead *) FuncObj;
	pc = (ByteCode *) PC+3; /* Skip over the miscn opcode we're in now */
      }
    else
      {
	fnhead = (struct fnhead *)
		    Addr68k_from_LADDR(0xFFFFFF & swapx((int)caller->fnheader));
	pc = (ByteCode *)fnhead+(caller->pc);
      }
newpc:
    opcode = (short)((unsigned char)GETBYTE((char *)pc));
    switch (opcode)
      {
	case opc_RETURN:
	case opc_SLRETURN:  prevcaller = caller;
			    caller = (FX2 *) (Stackspace+(int)(GETCLINK(caller)));
			    goto newframe;

	case opc_FN1:   if (MVLIST_index == Get_AtomNo(pc+1))
			 {
			   if (unbind_count > 0) 
			     simulate_unbind(caller, unbind_count, prevcaller);
			    /* would add 3 to PC, but miscn ret code does. */
#ifndef BIGATOMS
			   if (caller == immediate_caller) PC = pc;
#else
			   /* BUT 3's not enough for big atoms, so add 1 */
			   if (caller == immediate_caller) PC = pc + 1;
#endif /* BIGATOMS */

			   else caller->pc = (int)pc+ FN_OPCODE_SIZE-(int)fnhead;
			   return(args[0]);
			 }
			break;

	case opc_UNBIND:  pc += 1;
			  unbind_count += 1;
			  goto newpc;

	case opc_JUMPX:  {
			   register short displacement;
			   displacement = (short) (GETBYTE((char *)pc+1));
			   if (displacement >= 128) displacement -= 256;
			   pc += displacement;
			   goto newpc;
			 }

	case opc_JUMPXX:  {
			    register int displacement;
			    displacement = (int) Get_DLword(pc+1);
			    if (displacement >= 32768) displacement -= 65536;
			    pc += displacement;
			    goto newpc;
			  }
	default:  if ((opcode >= opc_JUMP) && (opcode < opc_FJUMP))
		    {
		      pc += 2 + opcode - opc_JUMP;
		      goto newpc;
		    }
      }

	/*****************************************/
	/* Default case:  Return a single value. */
	/*****************************************/

    if (Listp(args[0])) return(car(args[0]));
    else return(args[0]);
  }


/************************************************************************/
/*									*/
/*			m a k e _ v a l u e _ l i s t			*/
/*									*/
/*	Given a count of values to return, and a pointer to an		*/
/*	array containing the values, CONS up a list that contains	*/
/*	the values.  This is because MVs are really returned on		*/
/*	the stack as a list -- SHOULD BE CHANGED!			*/
/*									*/
/************************************************************************/

make_value_list(argcount, argarray)
  int argcount;
  LispPTR *argarray;
  {
    register LispPTR result = NIL_PTR;
    register int i;
    if (argcount == 0) return(NIL_PTR);
    for (i = argcount-1; i>=0; i--)
      {
	result = cons(argarray[i], result);
      }
    return(result);
  }



/************************************************************************/
/*									*/
/*		     s i m u l a t e _ u n b i n d			*/
/*									*/
/*	Simulate the effect of UNBIND on a frame, to back us up		*/
/*	to where we ought to be when we return multiple values.		*/
/*									*/
/*									*/
/*									*/
/************************************************************************/

simulate_unbind(frame, unbind_count, returner)
  FX2 *frame, *returner;
  int unbind_count;
  {
    int unbind;
    LispPTR *stackptr;
    DLword *nextblock;
    stackptr = (LispPTR *) (Stackspace+frame->nextblock);
    nextblock = (DLword *) stackptr;
    for (unbind = 0; unbind<unbind_count; unbind++)
      {
	register int value;
	register LispPTR *lastpvar;
	int bindnvalues;
	for (;((int)*--stackptr>=0);); /* find the binding mark */
	value = (int)*stackptr;
	lastpvar = (LispPTR *) ((DLword *)frame + FRAMESIZE + 2 + GetLoWord(value));;
	bindnvalues = (~value)>>16;
	for(value=bindnvalues; --value >= 0;){*--lastpvar = 0xffffffff;}
	/* This line caused \NSMAIL.READ.HEADING to smash memory, */
	/* so I removed it 21 Jul 91 --JDS.  This was the only	  */
	/* difference between this function and the UNWIND code   */
	/* in inlineC.h						  */
/*	MAKEFREEBLOCK(stackptr, (DLword *)stackptr-nextblock); */
      }
    if (returner) returner->fast = 0;	/* since we've destroyed congituity
					/* in the stack, but that only
					   matters if there's a return. */
  }
