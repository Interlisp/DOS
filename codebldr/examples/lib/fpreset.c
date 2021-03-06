/* FPRESET.C: This program uses signal to set up a routine for handling
 * floating-point errors.
 */

#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <string.h>

jmp_buf mark;                       /* Address for long jump to jump to */
int  fperr;                         /* Global error number */

void fphandler( int sig, int num ); /* Prototypes */
void fpcheck( void );

void main()
{
    double n1, n2, r;
    int jmpret;

    /* Set up floating point error handler. */
    if( signal( SIGFPE, fphandler ) == SIG_ERR )
    {
        fprintf( stderr, "Couldn't set SIGFPE\n" );
        abort();
    }

    /* Save stack environment for return in case of error. First time
     * through, jmpret is 0, so true conditional is executed. If an
     * error occurs, jmpret will be set to -1 and false conditional
     * will be executed.
     */
    jmpret = setjmp( mark );
    if( jmpret == 0 )
    {
        printf( "Test for invalid operation - " );
        printf( "enter two numbers: " );
        scanf( "%lf %lf", &n1, &n2 );

        r = n1 / n2;
        /* This won't be reached if error occurs. */
        printf( "\n\n%4.3g / %4.3g = %4.3g\n", n1, n2, r );

        r = n1 * n2;
        /* This won't be reached if error occurs. */
        printf( "\n\n%4.3g * %4.3g = %4.3g\n", n1, n2, r );
    }
    else
        fpcheck();
}

/* Handles SIGFPE (floating point error) interrupt. */
void fphandler( int sig, int num )
{
    /* Set global for outside check since we don't want to do I/O in the
     * handler.
     */
    fperr = num;

    /* Initialize floating-point package. */
    _fpreset();

    /* Restore calling environment and jump back to setjmp. Return -1
     * so that setjmp will return false for conditional test.
     */
    longjmp( mark, -1 );
}

void fpcheck()
{
    char fpstr[30];

    switch( fperr )
    {
        case FPE_INVALID:
            strcpy( fpstr, "Invalid number" );
            break;

        case FPE_OVERFLOW:
            strcpy( fpstr, "Overflow" );
            break;

        case FPE_UNDERFLOW:
            strcpy( fpstr, "Underflow" );
            break;

        case FPE_ZERODIVIDE:
            strcpy( fpstr, "Divide by zero" );
            break;

        default:
            strcpy( fpstr, "Other floating point error" );
            break;
    }
    printf( "Error %d: %s\n", fperr, fpstr );
}
