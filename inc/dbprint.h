/* @(#) dbprint.h Version 1.8 (4/21/92). copyright Venue & Fuji Xerox  */


/************************************************************************/
/*									*/
/*	Copyright 1989, 1990 Venue, Fuji Xerox Co., Ltd, Xerox Corp.	*/
/*									*/
/*	This file is work-product resulting from the Xerox/Venue	*/
/*	Agreement dated 18-August-1989 for support of Medley.		*/
/*									*/
/************************************************************************/

#include <stdio.h>

/*  ================================================================  */
/*  Debugprint usage:  DBPRINT( paren'ed arglist )

    e.g.  DBPRINT ( ("value of foo is %d\n", foo) );

    the double parens are needed because of cpp's limited macro
    capability (can only handle variable number of args if they
    are paren'ed.  The motivation for this macro is, its easier to
    read:
            DBPRINT ( ("value of foo is %d\n", foo) );
    than:
	#ifdef DEBUG
            printf("value of foo is %d\n", foo);
	#endif

    e.g.  TRACER(expr);

    executes the expression if TRACE is on.
/*  ================================================================  */

	/* For debugging print statements */

#ifdef DEBUG
#define DBPRINT(X)   printf X; fflush(stdout)
#define DEBUGGER(X)  X;
#else
#define DBPRINT(X)
#define DEBUGGER(X)
#endif


	/* For trace print statements */

#ifdef TRACE
#define TPRINT(X)   printf X; fflush(stdout);
#define TRACER(X)  X;
#else /* TRACE */
#define TPRINT(X)
#define TRACER(X)
#endif /* TRACE */



	/* For tracing individual opcode executions */

#ifdef OPTRACE
#define OPTPRINT(X)   printf X; fflush(stdout);
#define OPTRACER(X)  X;
#else
#define OPTPRINT(X)
#define OPTRACER(X)
#endif


	/* For tracing function calls */

#ifdef FNTRACE
#define FNTPRINT(X)   printf X; fflush(stdout);
#define FNTRACER(X)  X;
#else
#define FNTPRINT(X)
#define FNTRACER(X)
#endif


	/* For function-call & return stack checking */

#ifdef FNSTKCHECK
#define FNCHKPRINT(X)   printf X; fflush(stdout);
#define FNCHECKER(X)  X;
#else
#define FNCHKPRINT(X)
#define FNCHECKER(X)
#endif

