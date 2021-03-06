/* CWAIT.C: This program launches several child processes and waits
 * for a specified process to finish.
 */

#define INCL_NOPM
#define INCL_NOCOMMON
#define INCL_DOSPROCESS
#ifndef _INTELC32_
#include <os2.h>        /* DosSleep */
#endif
#include <process.h>    /* cwait    */
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
        /* Spawn children in numeric order. */
        for( c = 0; c < 4; c++ )
            child[c].pid = spawnl( P_NOWAIT, argv[0], argv[0],
                                   child[c].name, NULL );

        /* Wait for randomly specified child, and respond when done. */
        c = getrandom( 0, 3 );
        printf( "Come here, %s\n", child[c].name );
        cwait( &termstat, child[c].pid, WAIT_CHILD );
        printf( "Thank you, %s\n", child[c].name );
    }

    /* If there are arguments, this must be a child. */
    else
    {
        /* Delay for a period determined by process number. */
        DosSleep( (argv[1][0] - 'A' + 1) * 1000L );
        printf( "Hi, dad. It's %s.\n", argv[1] );
    }
#endif
}
