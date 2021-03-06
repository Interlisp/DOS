/* WAIT.C: This program launches several child processes and waits for
 * the first to finish.
 */

#define INCL_NOPM
#define INCL_NOCOMMON
#define INCL_DOSPROCESS
#ifndef _INTELC32_
#include <os2.h>        /* DosSleep   */
#endif
#include <process.h>    /* wait       */
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

/* Macro to get a random integer within a specified range */
#define getrandom( min, max ) ((rand() % (int)(((max) + 1) - (min))) + (min))

struct  CHILD
{
    int     pid;
    char    name[10];
} child[4] = { { 0, "Ann" }, { 0, "Beth"  }, { 0, "Carl" }, { 0, "Dave" } };

void main( int argc, char *argv[] )
{
#ifdef _INTELC32_
    printf ( "OS/2 functions not available on DOS\n" );
#else
    int     termstat, pid, c, tmp;


    srand( (unsigned)time( NULL ) );               /* Seed randomizer */
    /* If no arguments, this is the parent. */
    if( argc == 1 )
    {

        /* Spawn children in random order with a random delay. */
        tmp = getrandom( 0, 3 );
        for( c = tmp; c < tmp + 4; c++ )
            child[c % 4].pid = spawnl( P_NOWAIT, argv[0], argv[0],
                                   child[c % 4].name, NULL );

        /* Wait for the first children. Only get ID of first. */
        printf( "Who's first?\n" );
        pid = wait( &termstat );
        for( c = 0; c < 3; c++ )
            wait( &termstat );

        /* Check IDs to see who was first. */
        for( c = 0; c < 4; c++ )
            if( pid == child[c].pid )
                printf( "%s was first\n\n", child[c].name );
    }

    /* If there are arguments, this must be a child. */
    else
    {
        /* Delay for random time. */
        srand( (unsigned)time( NULL ) * argv[1][0] );
        DosSleep( getrandom( 1, 5) * 1000L );
        printf( "Hi, dad. It's %s.\n", argv[1] );
    }
#endif
}
