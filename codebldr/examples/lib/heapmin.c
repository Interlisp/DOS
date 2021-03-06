/* HEAPMIN.C: This program illustrates heap management using
 * _heapadd and _heapmin.
 */

#include <stdio.h>
#include <conio.h>
#include <process.h>
#include <malloc.h>

void heapdump( char *msg );     /* Prototype */

char s1[] = { "Here are some strings that we use at first, then don't\n" };
char s2[] = { "need any more. We'll give their space to the heap.\n" };

void main()
{
    int *p[3], i;

    printf( "%s%s", s1, s2 );
    heapdump( "Initial heap" );

    /* Give space of used strings to heap. */
    _heapadd( s1, sizeof( s1 ) );
    _heapadd( s2, sizeof( s2 ) );
    heapdump( "After adding used strings" );

    /* Allocate some blocks. Some may use string blocks from _heapadd. */
    for( i = 0; i < 2; i++ )
        if( (p[i] = (int *)calloc( 10 * (i + 1), sizeof( int ) )) == NULL )
        {
            --i;
            break;
        }
    heapdump( "After allocating memory" );

    /* Free some of the blocks. */
    free( p[0] );
    free( p[1] );
    heapdump( "After freeing memory" );

    /* Minimize heap. */
    _heapmin();
    heapdump( "After compacting heap" );
}

/* Walk through heap entries, displaying information about each block. */
void heapdump( char *msg )
{
    struct _heapinfo hi;

    printf( "%s\n", msg );
    hi._pentry = NULL;
    while( _heapwalk( &hi ) == _HEAPOK )
        printf( "\t%s block at %Fp of size %u\t\n",
                hi._useflag == _USEDENTRY ? "USED" : "FREE",
                hi._pentry,
                hi._size );
    getch();
}
