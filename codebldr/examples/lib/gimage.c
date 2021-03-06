/* GIMAGE.C: This example illustrates animation routines including:
 *          _imagesize     _getimage     _putimage
 */

#include <conio.h>
#include <stddef.h>
#include <stdlib.h>
#include <malloc.h>
#include <graph.h>

short action[5]  = { _GPSET,   _GPRESET, _GXOR,    _GOR,     _GAND     };
char *descrip[5] = {  "PSET  ", "PRESET", "XOR   ", "OR    ", "AND   " };

void exitfree( char _huge *buffer );

void main()
{
    char  _huge *buffer;   /* Far pointer (with _fmalloc) could be used. */
    long  imsize;
    short i, x, y = 30;

    if( !_setvideomode( _MAXRESMODE ) )
        exit( 1 );

    /* Measure the image to be drawn and allocate memory for it. */
    imsize = (size_t)_imagesize( -16, -16, +16, +16 );
    buffer = halloc( imsize, sizeof( char ) );
    if ( buffer == (char _far *)NULL )
        exit( 1 );

    _setcolor( 3 );
    for ( i = 0; i < 5; i++ )
    {
        /* Draw ellipse at new position and get a copy of it. */
        x = 50; y += 40;
        _ellipse( _GFILLINTERIOR, x - 15, y - 15, x + 15, y + 15 );
        _getimage( x - 16, y - 16, x + 16, y + 16, buffer );
        if( _grstatus() )
            exitfree( buffer );        /* Quit on error                   */

        /* Display action type and copy a row of ellipses with that type. */
        _settextposition( 1, 1 );
        _outtext( descrip[i] );
        while( x < 260 )
        {
            x += 5;
            _putimage( x - 16, y - 16, buffer, action[i] );
            if( _grstatus() < 0 )      /* Ignore warnings, quit on errors. */
                exitfree( buffer );
        }
        getch();
    }
    exitfree( buffer );
}

void exitfree( char _huge *buffer )
{
    hfree( buffer );
    exit( !_setvideomode( _DEFAULTMODE ) );
}
