/* MOVETO.C: This program draws line segments of different colors. */

#include <graph.h>
#include <stdlib.h>
#include <conio.h>

void main()
{
   short x, y, xinc, yinc, color = 1;
   struct videoconfig v;

   /* Find a valid graphics mode. */
   if( !_setvideomode( _MAXCOLORMODE ) )
      exit( 1 );
   _getvideoconfig( &v );
   xinc = v.numxpixels / 50;
   yinc = v.numypixels / 50;

   for( x = 0, y = v.numypixels - 1; x < v.numxpixels; x += xinc, y -= yinc )
   {
      _setcolor( color++ % 16 );
      _moveto( x, 0 );
      _lineto( 0, y );
   }
   getch();

   _setvideomode( _DEFAULTMODE );
}
