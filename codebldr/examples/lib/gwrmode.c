/* GWRMODE.C: This program illustrates _getwritemode and _setwritemode. */

#include <conio.h>
#include <stdlib.h>
#include <graph.h>

short wmodes[5]  = { _GPSET,   _GPRESET, _GXOR,    _GOR,     _GAND    };
char *wmstr[5]   = { "PSET  ", "PRESET", "XOR   ", "OR    ", "AND   " };

void box( short x, short y, short size, short writemode, short fillmode );

void main()
{
   short i, x, y;

   /* Find a valid graphics mode. */
   if( !_setvideomode( _MAXCOLORMODE ) )
      exit( 1 );

   x = y = 70;
   box( x, y, 50, _GPSET, _GFILLINTERIOR );
   _setcolor( 2 );
   box( x, y, 40, _GPSET, _GFILLINTERIOR );
   for( i = 0; i < 5; i++ )
   {
      _settextposition( 1, 1 );
      _outtext( wmstr[i] );
      box( x += 12, y += 12, 50, wmodes[i], _GBORDER );
      getch();
   }
   _setvideomode( _DEFAULTMODE );
}

void box( short x, short y, short size, short writemode, short fillmode )
{
    short wm, side;

    wm = _getwritemode();           /* Save write mode and set new. */
    _setwritemode( writemode );
    _rectangle( fillmode, x - size, y - size, x + size, y + size );
    _setwritemode( wm );            /* Restore original write mode. */
}
