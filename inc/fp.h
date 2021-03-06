/* @(#) fp.h Version 1.13 (4/21/92). copyright Venue & Fuji Xerox  */


/************************************************************************/
/*									*/
/*	Copyright 1989, 1990 Venue, Fuji Xerox Co., Ltd, Xerox Corp.	*/
/*									*/
/*	This file is work-product resulting from the Xerox/Venue	*/
/*	Agreement dated 18-August-1989 for support of Medley.		*/
/*									*/
/************************************************************************/

#ifdef DOS
#include <i32.h>
#endif /* DOS */

#ifdef DEC3100
#include <ieeefp.h>
#endif /* DEC3100 */

#ifdef RISCOS
/*#include <ieeefp.h> */
/*#define isnan isnand */
#define isnan(x) 0
#endif /* RISCOS */


/*  --------------------------------------------------
    FPCLEAR         - clear status as necessary
    FPTEST(result)  - check result or status

    Sun 4 compiler w. -O2 moves too much code around
    to use FLTINT.
    --------------------------------------------------  */

#ifdef FLTINT
extern int  FP_error;

/*  Note that a compiler may very likely move code around the arithmetic
    operation, causing this test (set by an interrupt handler) to be
    incorrect.  For example, the Sun SPARC compiler with -O2 makes
    this test incorrect.
 */

#define FPCLEAR         FP_error = 0;
#define FPTEST(result)  FP_error

#elif (defined(sparc) || defined(I386) || defined(HPUX))

/*  fpstatus_ is a FORTRAN library routine (in libc) which
    can be called to determine floating point status results.
    Documented in the Sun manual, "Floating Point Programmer's Guide",
    (Rev. A 19-Sep-86), pg. 34, it does *not* exist in libc for the
    SPARC.

    For sparc, should also check for isnan?  Don't know what isnormal
    & issubnormal do (these are sunos4.0 only)
 */

#define FPCLEAR
#define FPTEST(result) (isinf(result) || isnan(result))

#elif AIX
#define FPCLEAR
#define FPTEST(result) ((!finite(result)) || isnan(result))
#elif DOS
#define FPCLEAR
#define FPTEST(result) (_getrealerror() | I87_ZERO_DIVIDE | I87_OVERFLOW | I87_UNDERFLOW)
#else
static int constant0 = 0;
unsigned int fpstatus_();

#define FPCLEAR         fpstatus_(&constant0);
#define FPTEST(result) (fpstatus_(&constant0) & 0xF0)

#endif /* FLTINT */



/************************************************************************/
/*									*/
/*		Sun 386i Floating-point accelerator (80387)		*/
/*			Initialization macros				*/
/*									*/
/*	I386Reset -- set up so convert-to-integer truncates		*/
/*	I386Round -- set up so FP operation round properly		*/
/*									*/
/************************************************************************/

#ifdef I386
#define I386Reset			\
  {	/* 80387 initialization */ 	\
    asm(".text");			\
    asm("fldcw __FP_trunc");		\
  }

#define I386Round		\
  {				\
    asm(".text");		\
    asm("fldcw __FP_round");	\
  }

#endif /* I386 */


