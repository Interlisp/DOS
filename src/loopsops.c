/* @(#) loopsops.c Version 1.7 (4/23/92). copyright venue & Fuji Xerox  */
static char *id = "@(#) loopsops.c	1.7 4/23/92	(venue & Fuji Xerox)";


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


/*
  LOOPS opcodes:

	FetchMethodOrHelp(object, selector) -> MethodFn
	LookupIV(object, iv) -> Index into iDescrs
	GetValue(object, iv)
	PutValue(object, iv, val)
*/

#include "lispemul.h"
#include "lsptypes.h"
#include "cell.h"
#include "lispmap.h"
#include "lspglob.h"
#include "adr68k.h"
#include "stack.h"
#include "gc.h"

LispPTR get_package_atom();
static char il_string[]="INTERLISP";
#define GET_IL_ATOM(string)	get_package_atom((string),(sizeof(string)-1), \
						 il_string,9,NIL)

#define AtomValPtr(index)	Addr68k_from_LADDR(*(GetVALCELL68k(index)))

#define DTD_FROM_LADDR(x)						\
	(((struct dtd *)GetDTD(GetTypeNumber((x))))->dtd_namelo +	\
	 (((struct dtd *)GetDTD(GetTypeNumber((x))))->dtd_namehi<<16))

/* These assume 0 <= POSINT <= 65535 */
#define SMALLP_FROM_POSINT(x)	((x) | S_POSITIVE)
#define POSINT_FROM_SMALLP(x)	((x) & 0xffff)

#define RETCALL(fn,argnum)	return lcfuncall(fn,argnum,3)

/* #define PUNT	(-1)*/

#define METH_CACHE_INDEX(CLASS,SELECTOR)	(1023 & ((CLASS) ^ (SELECTOR)))
#define IV_CACHE_INDEX(VARLIST,IV)		(1023 & ((VARLIST) ^ (IV)))

#define LC_TYPEP(obj,typeATOM)	(DTD_FROM_LADDR((obj)) == typeATOM)

#define INSTANCEP(obj)	(LC_TYPEP((obj),atom_instance))
#define CLASSP(obj)	(LC_TYPEP((obj),atom_class))

#define INSTANCE_OR_PUNT(obj,fn,argnum)					\
	{ if (!LC_TYPEP((obj),atom_instance))				\
	    RETCALL(fn,argnum);						\
	}

#define INSTANCE_CLASS_OR_PUNT(obj,fn,argnum)				\
	{ register LispPTR tmp = DTD_FROM_LADDR(obj);			\
	  if (tmp != atom_instance && tmp != atom_class)		\
	    RETCALL(fn,argnum);						\
	}

#define LC_INIT	if (atom_instance == 0) LCinit()


#define GET_IV_INDEX(objptr,iv,dest,otherwise)				\
{									\
  register struct LCIVCacheEntry *ce;					\
  register LispPTR iNames=objptr->iNames;				\
									\
  ce = &(LCIVCache[IV_CACHE_INDEX(iNames, iv)]);			\
  if (ce->iNames == iNames && ce->iv == iv){				\
    (dest) = POSINT_FROM_SMALLP(ce->index);				\
  }else{								\
    if (! Listp(iNames)) {						\
      otherwise;							\
    }else{								\
      register int i=0;							\
      while (1) {							\
	if ( car(iNames) == iv) {					\
	  ce->iNames = objptr->iNames;					\
	  ce->iv = iv;							\
	  ce->index = SMALLP_FROM_POSINT(i);				\
	  (dest) = i;							\
	  break;							\
	}else{								\
	  i++;								\
	  if ((iNames = cdr(iNames)) == NIL_PTR) {			\
	    otherwise;							\
	  }								\
	}								\
      }									\
    }									\
  }									\
}


struct LCClass {		/* class datatype */
  LispPTR metaClass, ivNames, ivDescrs,
	  classUnitRec, localIVs, cvNames,
	  cvDescrs, className, supers, subClasses,
	  otherClassDescription, selectors, methods, localSupers;
};

struct LCInstance {		/* instance datatype */
  LispPTR class, iNames, iDescrs, instMiscField;
};

struct LCMethodCacheEntry {
  LispPTR class, selector, method_fn, junk;
} *LCMethodCache;

struct LCIVCacheEntry {
  LispPTR iNames, iv, index, junk;
} *LCIVCache;

LispPTR atom_instance = 0,	/* various atom indices */
	atom_class,
	atom_annotatedValue,
	atom_FetchMethodOrHelp_LCUFN,
	atom_FetchMethod_LCUFN,
	atom_FindVarIndex_LCUFN,
	atom_GetIVValue_LCUFN,
	atom_PutIVValue_LCUFN;

/* Called once to initialize the "constants" above */

LCinit() {
  atom_instance = GET_IL_ATOM("instance");
  atom_class = GET_IL_ATOM("class");
  atom_annotatedValue = GET_IL_ATOM("annotatedValue");
  atom_FetchMethodOrHelp_LCUFN = GET_IL_ATOM("\\FetchMethodOrHelp-LCUFN");
  atom_FetchMethod_LCUFN = GET_IL_ATOM("\\FetchMethod-LCUFN");
  atom_FindVarIndex_LCUFN = GET_IL_ATOM("\\FindVarIndex-LCUFN");
  atom_GetIVValue_LCUFN = GET_IL_ATOM("\\GetIVValue-LCUFN");
  atom_PutIVValue_LCUFN = GET_IL_ATOM("\\PutIVValue-LCUFN");
  LCMethodCache = (struct LCMethodCacheEntry *)
			AtomValPtr(GET_IL_ATOM("*Global-Method-Cache*"));
  LCIVCache = (struct LCIVCacheEntry *)
			AtomValPtr(GET_IL_ATOM("*Global-IV-Cache-Block*"));
  return NIL_PTR;		/* in case called from lisp */
}

/* Type check fn */
/* We only check for instance and class, neither of which has supertypes,
   so the loop is unnecessary.  */
/* Not used
int LCTypeOf(thing, typename)
register LispPTR thing,typename;
{
  register struct dtd *dtd68k;
  
  for(dtd68k=(struct dtd *) GetDTD(GetTypeNumber(thing));
      typename	!= dtd68k->dtd_namelo+(dtgd68k->dtd_namehi<<16) ;
      dtd68k=(struct dtd *) GetDTD(dtd68k->dtd_supertype) ) {
    if( dtd68k->dtd_supertype == 0) return 0;
  }
    return 1;
}
*/


/* Method lookup using global cache */

LispPTR LCFetchMethodOrHelp(object, selector)
     register LispPTR object, selector;
{
  register struct LCInstance *objptr;
  register struct LCMethodCacheEntry *ce;
  register LispPTR cur_class;
  
  LC_INIT;

  INSTANCE_CLASS_OR_PUNT(object,atom_FetchMethodOrHelp_LCUFN,2);

  objptr = (struct LCInstance *)  Addr68k_from_LADDR(object);
  ce = &(LCMethodCache[METH_CACHE_INDEX((cur_class = objptr->class), selector)]);
  if(ce->class == cur_class && ce->selector == selector)
    return ce->method_fn;
  
  /* not in cache, search class then supers */
  {
    LispPTR supers = ((struct LCClass *)Addr68k_from_LADDR(cur_class))->supers;
    
    for (;;) {
      register i=0;
      register LispPTR val;
      register LispPTR *selectorptr;
      register struct LCClass *classptr;
      
      classptr = (struct LCClass *)Addr68k_from_LADDR(cur_class);
      if (classptr->selectors == NIL_PTR) {
	goto next_class;
      }else{
	selectorptr = (LispPTR *)Addr68k_from_LADDR(classptr->selectors);
      }
      
      while ((val = selectorptr[i++]) != NIL_PTR) {
	if (val == selector) {
	  ce->class = objptr->class;
	  ce->selector = selector;
	  return (ce->method_fn =
		   ((LispPTR *)Addr68k_from_LADDR(classptr->methods))[i-1]) ;
	}
      };
    
next_class:
      if( (cur_class = car(supers) ) == NIL_PTR)
	break;
      supers = cdr(supers);
    }
  }

  /* we didn't find it at all; punt */
  RETCALL(atom_FetchMethodOrHelp_LCUFN,2);

/*  return lcfuncall(atom_FetchMethodOrHelp_LCUFN,2,3); */
/*  return PUNT;*/
}


LispPTR LCFetchMethod(class, selector)
     register LispPTR class, selector;
{
  register struct LCMethodCacheEntry *ce;
  
  LC_INIT;

  /* Check cache before doing type check */
  ce = &(LCMethodCache[METH_CACHE_INDEX(class, selector)]);
  if(ce->class == class && ce->selector == selector)
    return ce->method_fn;
  
  /* it wasn't there, go search class then supers */
  
  if (!LC_TYPEP(class, atom_class)) RETCALL(atom_FetchMethod_LCUFN,2);
  {
    register LispPTR cur_class = class;
    LispPTR supers = ((struct LCClass *)Addr68k_from_LADDR(cur_class))->supers;
    
    for (;;) {
      register i=0;
      register LispPTR val;
      register struct LCClass *classptr;
      register LispPTR *selectorptr;
      
      classptr = (struct LCClass *)Addr68k_from_LADDR(cur_class);
      if (classptr->selectors == NIL_PTR) goto next_class;
      else selectorptr = (LispPTR *)Addr68k_from_LADDR(classptr->selectors);
      
      while ((val = selectorptr[i++]) != NIL_PTR) {
	if (val == selector) {
	  ce->class = class;
	  ce->selector = selector;
	  return (ce->method_fn =
		  ((LispPTR *)Addr68k_from_LADDR(classptr->methods))[i-1]);
	}
      };
    
next_class:
      if( (cur_class = car(supers) ) == NIL_PTR)
	break;
      supers = cdr(supers);
    }
  }

  /* we didn't find it at all; return NIL */
  return NIL_PTR;
}

LispPTR LCFindVarIndex(iv, object)
  register LispPTR object, iv;
{
  register struct LCInstance *objptr;
  register struct LCIVCacheEntry *ce;
  register LispPTR iNames;

  LC_INIT;

  INSTANCE_CLASS_OR_PUNT(object,atom_FindVarIndex_LCUFN,2);

  objptr = (struct LCInstance *) Addr68k_from_LADDR(object);
  ce = &(LCIVCache[IV_CACHE_INDEX((iNames = objptr->iNames), iv)]);
  if (ce->iNames == iNames && ce->iv == iv)
    return ce->index;

  if (! Listp(iNames)) return NIL_PTR;	/* FastFindIndex lisp macro (& others?) */
					/* needs this check too ! */
  {
    register int i;

    for (i=0; i++; (iNames = cdr(iNames)) != NIL_PTR) {
      if ( car(iNames) == iv) {
	ce->iNames = objptr->iNames;
	ce->iv = iv;
	return (ce->index = SMALLP_FROM_POSINT(i));
      }
    }
    return NIL_PTR;
  }
}


#if 01

LispPTR
LCGetIVValue(object, iv)
  register LispPTR object, iv;
{
  register struct LCInstance *objptr;
  register LispPTR val;
  register int index;

  LC_INIT;
  INSTANCE_OR_PUNT(object,atom_GetIVValue_LCUFN,2);

  objptr = (struct LCInstance *) Addr68k_from_LADDR(object);
  GET_IV_INDEX(objptr,iv,index,goto pnut);
  val = ((LispPTR *) Addr68k_from_LADDR(objptr->iDescrs))[index];
  if (!LC_TYPEP(val, atom_annotatedValue)) return val;
 pnut:
  RETCALL(atom_GetIVValue_LCUFN,2);
/*
  return LCGetActiveValue(object,val);
*/
}

LCPutIVValue(object, iv, val)
  register LispPTR object, iv, val;
{
  register struct LCInstance *objptr;
  register LispPTR *valptr;
  register int index;

  LC_INIT;
  INSTANCE_OR_PUNT(object,atom_PutIVValue_LCUFN,3);

  objptr = (struct LCInstance *) Addr68k_from_LADDR(object);
  GET_IV_INDEX(objptr,iv,index,goto pnut);
  valptr = &(((LispPTR *) Addr68k_from_LADDR(objptr->iDescrs))[index]);
  if (!LC_TYPEP(*valptr, atom_annotatedValue)) {
    FRPLPTR((*valptr), val);
    return val;
  }
 pnut:
  RETCALL(atom_PutIVValue_LCUFN,3);
/*
  return LCPutActiveValue(object,*valptr,val);
*/
}

#endif



lcfuncall(atom_index,argnum,bytenum)
     register unsigned int atom_index;   /* Atomindex for Function to invoke */
     register int argnum;   /* Number of ARGS on TOS and STK */
     register int bytenum;  /* Number of bytes of Caller's
			       OPCODE(including multi-byte) */
{
  register struct definition_cell *defcell68k; /* Definition Cell PTR */
  register short pv_num;	/* scratch for pv */
  register struct fnhead *tmp_fn;
  int rest;			/* use for arignments */
  int closurep=NIL;
  register CClosure *closure68k;

  if (atom_index == 0xffffffff) error("Loops punt to nonexistant fn");

  /* Get Next Block offset from argnum */
  CURRENTFX->nextblock = (LADDR_from_68k(CurrentStackPTR) & 0x0ffff)
			 - (argnum<<1)+4 /* +3  */;

  /* Setup IVar */
  IVar = Addr68k_from_LADDR((((LispPTR)(CURRENTFX->nextblock)) | STK_OFFSET));

  /* Set PC to the Next Instruction and save into FX */
  CURRENTFX->pc = ((unsigned int)PC - (unsigned int)FuncObj) + bytenum;

  PushCStack;			/* save TOS */

  /* Get DEFCELL 68k address */
  defcell68k =(struct definition_cell *) GetDEFCELL68k(atom_index);

  tmp_fn = (struct fnhead *)Addr68k_from_LADDR( defcell68k->defpointer );

  if((unsigned int)(CurrentStackPTR + tmp_fn->stkmin +STK_SAFE)
     >=  (unsigned int)EndSTKP )
    {
      LispPTR test;
      test= *((LispPTR*)CurrentStackPTR);
      DOSTACKOVERFLOW(argnum,bytenum-1);
      S_CHECK(test==*((LispPTR*)CurrentStackPTR), "overflow in ccfuncall");
    }
  FuncObj = tmp_fn;

  if( FuncObj->na >= 0 )
    {
      /* This Function is Spread Type */
      /* Arguments on Stack Adjustment  */
      rest = argnum - FuncObj->na;

      while(rest <0) {
	PushStack(NIL_PTR);
	rest++;
      }
      CurrentStackPTR -= (rest<<1);
    } /* if end */

  /* Set up BF */
  CurrentStackPTR+=2;
  GETWORD(CurrentStackPTR) = BF_MARK;
  GETWORD(CurrentStackPTR+1) = CURRENTFX->nextblock;
  CurrentStackPTR+=2;

  /* Set up FX */
  GETWORD(CurrentStackPTR) = FX_MARK;

  /* Now SET new FX */
  ((struct frameex1 *)CurrentStackPTR)->alink = LADDR_from_68k(PVar);
  PVar = (DLword *)CurrentStackPTR + FRAMESIZE;
  ((struct frameex1 *)CurrentStackPTR)->lofnheader =
    (defcell68k->defpointer) & 0x0ffff;
  ((struct frameex1 *)CurrentStackPTR)->hi2fnheader =
    ((defcell68k->defpointer) & 0x0ff0000) >> 16;
  ((struct frameex1 *)CurrentStackPTR)->hi1fnheader = 0;

  CurrentStackPTR = PVar;

  /* Set up PVar area */
  pv_num = FuncObj->pv+1; /* Changed Apr.27 */

  while(pv_num > 0) {
    *((LispPTR *)CurrentStackPTR) = 0x0ffff0000;
    CurrentStackPTR += DLWORDSPER_CELL;
    *((LispPTR *)CurrentStackPTR) = 0x0ffff0000;
    CurrentStackPTR += DLWORDSPER_CELL;
    pv_num--;
  }

  /* Set PC points New Function's first OPCODE */
  PC = (ByteCode *)FuncObj + FuncObj->startpc;

  return -2;			/* signal to OP_miscn to leave stack & pc alone */
} /* end lcfuncall */

