/* BEGTHRD.C illustrates multiple threads using functions:
 *      _beginthread            _endthread
 *
 * Also the global variable:
 *      _threadid
 *
 * This program requires the multithread library. For example, compile
 * with the following command line:
 *      CL /MT THREADS.C
 */

#define INCL_NOCOMMON
#define INCL_NOPM
#define INCL_DOSPROCESS
#define INCL_VIO
#ifndef _INTELC32_
#include <os2.h>
#endif
#include <process.h>    /* _beginthread, _endthread */
#include <stddef.h>     /* _threadid                */
#include <stdlib.h>
#include <conio.h>
#ifdef _INTELC32_
#include <stdio.h>
#endif

void Bounce( int c );       /* Prototypes */
void CheckKey( void *dummy );

/* GetRandom returns a random integer between min and max. */
#define GetRandom( min, max ) ((rand() % (int)(((max) + 1) - (min))) + (min))

#define STACK_SIZE   4096

#ifndef _INTELC32_
BOOL repeat = TRUE;         /* Global repeat flag and video variable */
VIOMODEINFO vmi = { sizeof( VIOMODEINFO ) };
#endif

void main()
{
#ifdef _INTELC32_
    printf ( "OS/2 functions not available on DOS\n" );
#else
    PCHAR   stack;
    CHAR    ch = 'A';

    /* Get display screen's text row and column information. */
    VioGetMode( &vmi, 0 );

    /* Launch CheckKey thread to check for terminating keystroke. */
    _beginthread( CheckKey, NULL, STACK_SIZE, NULL );

    /* Loop until CheckKey terminates program. */
    while( repeat )
    {
        /* On first loops, launch character threads. */
        _beginthread( Bounce, NULL, STACK_SIZE, (void *)ch++ );

        /* Wait one second between loops. */
        DosSleep( 1000L );
    }
#endif
}

#ifndef _INTELC32_
/* CheckKey - Thread to wait for a keystroke, then clear repeat flag. */
void CheckKey( void *dummy )
{
    getch();
    repeat = 0;      /* _endthread implied */
}
#endif

#ifndef _INTELC32_
/* Bounce - Thread to create and control a colored letter that moves
 * around on the screen.
 *
 * Params: ch - the letter to be moved
 */
void Bounce( int ch )
{
    /* Generate letter and color attribute from thread argument. */
    char      blankcell[2] = { 0x20, 0x07 };
    char      blockcell[2] = { ch , (ch % 16) + 1 };
    int       xold, xcur, yold, ycur;
    BOOL      first = TRUE;

    /* Seed random number generator and get initial location. */
    srand( *_threadid );
    xcur = GetRandom( 0, vmi.col - 1 );
    ycur = GetRandom( 0, vmi.row - 1 );
    while( repeat )
    {
        /* Pause between loops. */
        DosSleep( 100L );

        /* Blank out our old position on the screen, and draw new letter. */
        if( first )
            first = FALSE;
        else
            VioWrtCellStr( blankcell, 2, yold, xold, 0 );
        VioWrtCellStr( blockcell, 2, ycur, xcur, 0 );

        /* Increment the coordinate for next placement of the block. */
        xold = xcur;
        yold = ycur;
        xcur += GetRandom( -1, 1 );
        ycur += GetRandom( -1, 1 );

        /* Correct placement (and beep) if about to go off the screen. */
        if( xcur < 0 )
            xcur = 1;
        else if( xcur == vmi.col )
            xcur = vmi.col - 2;
        else if( ycur < 0 )
            ycur = 1;
        else if( ycur == vmi.row )
            ycur = vmi.row - 2;

        /* If not at screen border, continue, otherwise beep. */
        else
            continue;
        DosBeep( (ch - 'A') * 100, 175 );
    }
    /* _endthread given (but not really needed) to terminate. */
    _endthread();
}
#endif
