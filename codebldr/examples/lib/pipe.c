/* PIPE.C: This program uses _pipe to pass streams of text to
 * child processes.
 */

#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <process.h>            /* _pipe */
#include <math.h>

enum PIPES { READ, WRITE };     /* Constants 0 and 1 for READ and WRITE */
#define NUMPROBLEM 8

void main( int argc, char *argv[] )
{
#ifdef _INTELC32_
    printf ( "OS/2 functions not available on DOS\n" );
#else
    int     hpipe[2];
    char    hstr[20];
    int     termstat, pid, problem, c;

    /* If no arguments, this is the parent. */
    if( argc == 1 )
    {
        /* Open a sets of pipes. */
        if( _pipe( hpipe, 256, O_BINARY ) == -1 )
            exit( 1 );

        /* Convert pipe read handle to string and pass as argument to
         * spawned child. Program spawns itself (argv[0]).
         */
        itoa( hpipe[READ],  hstr,  10 );
        if( spawnl( P_NOWAIT, argv[0], argv[0], hstr, NULL )  == -1 )
            printf( "Spawn failed" );

        /* Put problem in write pipe. Since child is running simultaneously,
         * first solutions may be done before last problem is given.
         */
        for( problem = 1000; problem <= NUMPROBLEM * 1000; problem += 1000 )
        {
            printf( "Son, what is the square root of %d?\n", problem );
            write( hpipe[WRITE], (char *)&problem, sizeof( int ) );
        }

        /* Wait until child is done processing. */
        wait( &termstat );
        if( termstat & 0xff )
            printf( "Child failed\n" );

        close( hpipe[READ] );
        close( hpipe[WRITE] );
    }

    /* If there is an argument, this must be the child. */
    else
    {
        /* Convert passed string handle to integer handle. */
        hpipe[READ] = atoi( argv[1] );

        /* Read problem from pipe and calculate solution. */
        for( c = 0; c < NUMPROBLEM; c++ )
        {
            read( hpipe[READ], (char *)&problem, sizeof( int ) );
            printf( "Dad, the square root of %d is %3.2f.\n",
                    problem, sqrt( (double)problem ) );;
        }
    }
#endif
}
