/* BHEAPSEG.C: This program C illustrates dynamic allocation of based
 * memory using functions _bheapseg, _bfreeseg, _bmalloc, and _bfree.
 */

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

void main()
{

#if _INTELC32_
#define _segment int
    _segment seg;
    char *outstr, *instr;
    char *pout, *pin;
#else
    _segment seg;
    char _based( seg ) *outstr, _based( seg ) *instr;
    char _based( seg ) *pout,   _based( seg ) *pin;
#endif
    char tmpstr[80];
    int  len;

    printf( "Enter a string: " );
    gets( tmpstr );

    /* Request a based heap. Use based so that memory won't be taken from
     * near heap.
     */
    if( (seg = _bheapseg( 1000 )) == _NULLSEG )
        exit( 1 );

    /* Allocate based memory for two strings. */
    len = strlen( tmpstr );
    if( ((instr  = _bmalloc( seg, len + 1 )) == _NULLOFF) ||
        ((outstr = _bmalloc( seg, len + 1 )) == _NULLOFF) )
        exit( 1 );

    /* Copy a lowercased string to dynamic memory. The based memory is
     * far when addressed as a whole.
     */
    _fstrlwr( _fstrcpy( (char _far *)instr, (char _far *)tmpstr ) );

    /* Copy input string to output string in reversed order. When reading
     * and writing individual characters from a based heap, the compiler will
     * try to process them as near, thus speeding up the processing.
     */
    for( pin = instr + len - 1, pout = outstr;
                pout < outstr + len; pin--, pout++ )
        *pout = *pin;
    *pout = '\0';

    /* Display strings. Again, strings as a whole are far. */
    printf( "Input:  %Fs\n", (char _far *)instr );
    printf( "Output: %Fs\n", (char _far *)outstr );

    /* Free blocks and release based heap. */
    _bfree( seg, instr );
    _bfree( seg, outstr );
    _bfreeseg( seg );
}
