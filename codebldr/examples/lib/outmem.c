/* OUTMEM.C illustrates:
 *    _outmem
 */

#include <stdio.h>
#include <graph.h>

void main()
{
    int  i, len;
    char tmp[10];

    _clearscreen( _GCLEARSCREEN );
    for( i = 0; i < 256; i++ )
    {
        _settextposition( (i % 24) + 1, (i / 24) * 7 );
        len = sprintf( tmp, "%3d %c", i, i );
        _outmem( tmp, len );
    }
    _settextposition( 24, 1 );
}
