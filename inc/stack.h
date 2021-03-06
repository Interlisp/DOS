/* @(#) stack.h Version 1.29 (3/2/90). copyright envos & Fuji Xerox  */

/*
 *	Author	:	Takeshi Shimizu
 *			Hiroshi Hayata
 */
/**********************************************************/
/*
		File Name :	stack.h

		Changed	:	Take(11-Dec-87)
				Take(06-Jan-88)
				JDS (09-Dec-88)

*/
/**********************************************************/


/************************************************************************/
/*									*/
/*	Copyright 1989, 1990 Venue, Fuji Xerox Co., Ltd, Xerox Corp.	*/
/*									*/
/*	This file is work-product resulting from the Xerox/Venue	*/
/*	Agreement dated 18-August-1989 for support of Medley.		*/
/*									*/
/************************************************************************/


/* ContextSW frame number */
#define CurrentFXP	0
#define ResetFXP	1
#define SubovFXP	2
#define KbdFXP		3
#define HardReturnFXP	4
#define GCFXP		5
#define FAULTFXP	6

#define STK_FSB_WORD	0xA000
#define STK_FSB_32	0xA0000000

#define STK_GUARD_WORD	0xE000
#define STK_GUARD_32	0xE0000000
#define  BF_MARK0	0x8000
#define  BF_MARK	0x8000
#define  BF_MARK32	0x80000000
#define  FX_MARK	0xc000
#define  FX_MARK_NATIVE 0xc800

#define STK_GUARD	7
#define STK_FX		6
#define STK_FSB		5
#define STK_BF		4
#define STK_NOTFLG	0

#define STK_SAFE		32	/* added with stkmin */
#define MINEXTRASTACKWORDS	32
#define STACKAREA_SIZE	768

/* For Fvar operations */
#define FVSTACK		2
#define FVGLOBAL	6
#define FVUBNBOUND	3
#define FVIVAR		0x0
#define FVPVAR		0x80
#define FVFVAR		0xC0
#define ENDSTACKMARK	0xb

#define FXPTR(base) ((struct frameex1 *) WORDPTR(base))

#ifndef BYTESWAP

	/*******************************************************/
	/*      Normal definitions for structures on stack     */
	/*******************************************************/
struct fnhead{
	DLword		stkmin;	/* ?? */
	short		na;	/* Numbers of arguments */
	short		pv;	/* ?? */
	DLword		startpc;
			/* head of ByteCodes, DLword offset from stkmin */
	unsigned	native		: 1;	/* native translated? */
	unsigned	nil1		: 1;	/* not used */
	unsigned	argtype		: 2;	/* ?? */
	unsigned	nil2		: 4;	/* not used */
	unsigned	framename	:24;	/* index in AtomSpace */
	DLword		ntsize;			/* size of NameTable */
	unsigned	nlocals		: 8;	/* ?? */
	unsigned	fvaroffset	: 8;
			/* DLword offset from head of NameTable */
	/* NameTable of variavle length is follwing with this structure. */
} ;


typedef struct frameex1{
	unsigned	flags	:3;
	unsigned	fast	:1;
	unsigned	native	:1;	/* This frame treats N-func */
	unsigned	incall	:1;
	unsigned	validnametable	:1;
			/* 0: look for FunctionHeader
			   1: look for NameTable on this FrameEx */
	unsigned	nopush	:1;
	unsigned	usecount :8;
	DLword	alink;		/* alink pointer (Low addr) */
	DLword	lofnheader;	/* pointer to FunctionHeader (Low addr) */
	unsigned hi1fnheader : 8; /* pointer to FunctionHeader (Hi1 addr) */
	unsigned hi2fnheader : 8; /* pointer to FunctionHeader (Hi2 addr) */
	DLword	nextblock;	/* pointer to FreeStackBlock */
	DLword	pc;		/* Program counter */
	DLword	lonametable;	/* ptr to NameTable of this FrameEx (Low addr) */
	unsigned hi1nametable :8;	/* ptr to NameTable of this FrameEx (Hi1 addr) */
	unsigned hi2nametable :8;	/* ptr to NameTable of this FrameEx (Hi2 addr) */
	DLword	blink;		/* blink pointer (Low addr) */
	DLword	clink;		/* clink pointer (Low addr) */
} FX;

typedef struct frameex2{
	unsigned	flags	:3;
	unsigned	fast	:1;
	unsigned	native	:1;	/* This frame treats N-func */
	unsigned	incall	:1;
	unsigned	validnametable	:1;
			/* 0: look for FunctionHeader
			   1: look for NameTable on this FrameEx */
	unsigned	nopush	:1;
	unsigned	usecount :8;
	DLword	alink;		/* alink pointer (Low addr) */
	LispPTR	fnheader;	/* pointer to FunctionHeader (swapped) */
	DLword	nextblock;	/* pointer to FreeStackBlock */
	DLword	pc;		/* Program counter */
	LispPTR	nametable;	/* address of NameTable (swapped) */
	DLword	blink;		/* blink pointer (Low addr) */
	DLword	clink;		/* clink pointer (Low addr) */
} FX2;




typedef struct fxblock {
	unsigned	flagbyte	: 8;
	unsigned	nil		: 23;
	unsigned	slowp		: 1;
} FXBLOCK;

typedef struct basic_frame {
	unsigned	flags	: 3;
	unsigned	nil	: 3;
	unsigned	residual: 1;
	unsigned	padding : 1;
	unsigned	usecnt  : 8;
	DLword		ivar;

} Bframe;

typedef struct stkword {
		unsigned	flags	 :3;
		unsigned	nil	 :5;
		unsigned	usecount :8;
 } StackWord;


 typedef struct stack_block {
		DLword flagword;
		DLword size;
 } STKBLK;

/* Lisp DATATYPE STACKP */
typedef struct stackp {
		DLword stackp0;
		DLword edfxp;
} STACKP;


	/*************************************************************/
	/*  Pointer-dereferencing macros for one-word structure ptrs */
	/*************************************************************/
#define BFRAMEPTR(ptr) ((Bframe *)(ptr))
#define STKWORDPTR(ptr) ((StackWord *)(ptr))

#else

	/*******************************************************/
	/*    Byte-swapped/Word-swapped definitions of stack   */
	/*******************************************************/
struct fnhead
  {
    short	na;	/* Numbers of arguments */
    DLword	stkmin;	/* ?? */
    DLword	startpc;
			/* head of ByteCodes, DLword offset from stkmin */
    short	pv;	/* ?? */
    unsigned	framename	:24;	/* index in AtomSpace */
    unsigned	nil2		: 4;	/* not used */
    unsigned	argtype		: 2;	/* ?? */
    unsigned	nil1		: 1;	/* not used */
    unsigned	native		: 1;	/* native translated? */
    unsigned	fvaroffset	: 8;
			/* DLword offset from head of NameTable */
    unsigned	nlocals		:8;	/* ?? */
    DLword	ntsize;		/* size of NameTable */
	/* NameTable of variavle length is follwing with this structure. */
  } ;


typedef struct frameex1{
	DLword	alink;		/* alink pointer (Low addr) */
	unsigned	usecount :8;
	unsigned	nopush	:1;
	unsigned	validnametable	:1;
			/* 0: look for FunctionHeader
			   1: look for NameTable on this FrameEx */
	unsigned	incall	:1;
	unsigned	native	:1;	/* This frame treats N-func */
	unsigned	fast	:1;
	unsigned	flags	:3; /* hi word */

	unsigned hi2fnheader : 8; /* pointer to FunctionHeader (Hi2 addr) */
	unsigned hi1fnheader : 8; /* pointer to FunctionHeader (Hi1 addr) */
	DLword	lofnheader;	/* pointer to FunctionHeader (Low addr) */

	DLword	pc;		/* Program counter */
	DLword	nextblock;	/* pointer to FreeStackBlock */

	unsigned hi2nametable :8;	/* pointer to NameTable of this FrameEx (Hi2 addr) */
	unsigned hi1nametable :8;	/* pointer to NameTable of this FrameEx (Hi1 addr) */
	DLword	lonametable;	/* pointer to NameTable of this FrameEx (Low addr) */

	DLword	clink;		/* clink pointer (Low addr) */
	DLword	blink;		/* blink pointer (Low addr) */
} FX;

typedef struct frameex2{
	DLword	alink;		/* alink pointer (Low addr) */
	unsigned	usecount :8;
	unsigned	nopush	:1;
	unsigned	validnametable	:1;
			/* 0: look for FunctionHeader
			   1: look for NameTable on this FrameEx */
	unsigned	incall	:1;
	unsigned	native	:1;	/* This frame treats N-func */
	unsigned	fast	:1;
	unsigned	flags	:3;

	LispPTR	fnheader;	/* pointer to FunctionHeader (swapped) */

	DLword	pc;		/* Program counter */
	DLword	nextblock;	/* pointer to FreeStackBlock */

	LispPTR	nametable;	/* address of NameTable (swapped) */

	DLword	clink;		/* clink pointer (Low addr) */
	DLword	blink;		/* blink pointer (Low addr) */
} FX2;




typedef struct fxblock {
	unsigned	slowp		: 1;
	unsigned	nil		: 23;
	unsigned	flagbyte	: 8;
} FXBLOCK;

typedef struct basic_frame {
	DLword		ivar;
	unsigned	usecnt  : 8;
	unsigned	padding : 1;
	unsigned	residual: 1;
	unsigned	nil	: 3;
	unsigned	flags	: 3;

} Bframe;

typedef struct stkword
  {
    unsigned	usecount :8;
    unsigned	nil	 :5;
    unsigned	flags	 :3;
  } StackWord;

typedef struct stack_block
  {
    DLword size;
    DLword flagword;
  } STKBLK;

/* Lisp DATATYPE STACKP */
typedef struct stackp
  {
    DLword edfxp;
    DLword stackp0;
  } STACKP;


	/*************************************************************/
	/*  Pointer-dereferencing macros for one-word structure ptrs */
	/*************************************************************/
#define BFRAMEPTR(ptr) ((Bframe *)(ptr))
#define STKWORDPTR(ptr) ((StackWord *) (2^(int)(ptr)))

#endif

#define STKWORD(stkptr) ((StackWord *)WORDPTR(stkptr))

#define FX_INVALIDP(fx68k) (((fx68k)==0) || ((DLword*)(fx68k)==Stackspace))
#define FX_size(fx68k)	(((FX*)(fx68k))->nextblock - LOLOC(LADDR_from_68k(fx68k)))
#define FSBP(ptr68k)	( ((STKBLK*)(ptr68k))->flagword == STK_FSB_WORD )
#define FSB_size(ptr68k)	(((STKBLK*)(ptr68k))->size)
/** Following suff assumes fx is 68kptr and val is LISP_LO_OFFSET **/
#define DUMMYBF(fx)		( ((DLword*)(fx))-DLWORDSPER_CELL )
#define SLOWP(fx)	(((FXBLOCK*)(fx))->slowp)
#define FASTP(fx)	(!SLOWP(fx))
#define SET_FASTP_NIL(fx68k) {  \
	if(FASTP(fx68k)){ \
		((FX*)fx68k)->blink=StkOffset_from_68K(DUMMYBF(fx68k));\
		((FX*)fx68k)->clink=((FX*)fx68k)->alink;\
		SLOWP(fx68k)=T; }}

#define GETALINK(fx)	((((fx)->alink) & 0xfffe)-FRAMESIZE)
#define SETALINK(fx,val) {if(FASTP(fx)) \
			   {((FX*)(fx))->blink=LADDR_from_68k(DUMMYBF(fx));\
			    ((FX*)(fx))->clink=((FX*)(fx))->alink;}\
			    ((FX*)(fx))->alink=(val)+FRAMESIZE+1;}

#define GETBLINK(fx) (SLOWP(fx) ? ((FX*)(fx))->blink : LOLOC(LADDR_from_68k(DUMMYBF(fx))))
#define SETBLINK(fx,val) { ((FX*)(fx))->blink =(val);\
			   if(FASTP(fx)){\
				((FX*)(fx))->clink=((FX*)(fx))->alink; \
				SLOWP(fx)=1;}}

#define GETCLINK(fx) (SLOWP(fx) ? (((FX*)(fx))->clink -FRAMESIZE):(((FX*)(fx))->alink -FRAMESIZE))
#define SETCLINK(fx,val) { ((FX*)(fx))->clink = (val) +FRAMESIZE;\
			if(FASTP((fx))){\
				((FX*)(fx))->blink=LADDR_from_68k(DUMMYBF(fx));\
				SLOWP(fx)=1;}}

#define SETACLINK(fx,val) {if(FASTP(fx)) \
				 {((FX*)(fx))->blink=LADDR_from_68k(DUMMYBF(fx));}\
				((FX*)(fx))->clink= (val) + FRAMESIZE;\
				((FX*)(fx))->alink= ((FX*)(fx))->clink +1; }

#define GETNAMETABLE(fx)					\
	((struct fnhead *) Addr68k_from_LADDR(swapx(		\
			((((FX2 *)fx)->validnametable)		\
			? ((FX2 *)fx)->nametable		\
			: ((FX2 *)fx)->fnheader			\
			)) & 0x00ffffff))

#define MAKEFREEBLOCK(ptr68k,size)      ( *((LispPTR*)(ptr68k))=STK_FSB_32| ((DLword)(size)) )
#define SETUPGUARDBLOCK(ptr68k,size)    ( *((LispPTR*)(ptr68k))=STK_GUARD_32| ((DLword)(size)) )




/************************************************************************/
/*									*/
/*			   Stack-checking macros			*/
/*									*/
/*	Enabled when STACKCHECK compile flag is set.			*/
/*									*/
/************************************************************************/

#ifdef STACKCHECK

#define S_CHECK(condition, msg)					\
  { 								\
    if(!(condition))						\
      {								\
	printf("\n\nStack check failed:  %s.\n\n", (msg));	\
	error("S_Check..");					\
      }								\
  }

#define S_WARN(condition, msg, scanptr)				\
  { 								\
    if(!(condition))						\
      {								\
	printf("\n\nStack check failed at 0x%x:  %s.\n\n", (scanptr), (msg));	\
      }								\
  }

#define CHECK_BF(bf68k) check_BF(bf68k)

#define CHECK_FX(fx68k) check_FX(fx68k)

#define PreMoveFrameCheck(fx68k)		\
  { LispPTR *tos_on_stack; \
    if(check_stack_rooms(fx68k) > 100)	\
      { \
	warn("moveframe:there is more than 100 words SPACE for FX"); \
	printf("# When calling "); \
	tos_on_stack=(LispPTR*)Addr68k_from_StkOffset((fx68k)->nextblock - 2); \
	print_atomname(*tos_on_stack); \
	printf("\n"); \
	stack_check(0); \
      }			\
  }

#else /* STACKCHECK */

#define S_CHECK(condition,msg)	{}
#define S_WARN(condition, msg, scanptr) {}
#define PreMoveFrameCheck(fx68k) {}
#define CHECK_BF(bf68k) {}
#define CHECK_FX(fx68k) {}

#endif /* STACKCHECK */




#define STK_MIN(fnobj) ((fnobj->stkmin /* NOT NEEDED in stkmin +STK_SAFE */) << 1)

#define STK_END_COMPUTE(stk_end,fnobj)					\
		( (int)stk_end - STK_MIN(fnobj) )


#define CLR_IRQ								\
	{Irq_Stk_Check = STK_END_COMPUTE((Irq_Stk_End = (int) EndSTKP), \
					 FuncObj);			\
	}
