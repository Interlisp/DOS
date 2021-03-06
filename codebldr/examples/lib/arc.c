/* ARC.C: This program draws a simple arc */

#include <graph.h>
#include <stdlib.h>
#include <conio.h>

void main()
{
   short x, y;
   struct xycoord xystart, xyend, xyfill;

   /* Find a valid graphics mode */
   if( !_setvideomode( _MAXRESMODE ) )
      exit( 1 );

   /* Draw arcs         */
   x = 100; y = 100;
   _arc( x - 60, y - 60, x, y, x - 30, y - 60, x - 60, y - 30 );
   _arc( x + 60, y + 60, x, y, x,      y + 30, x + 30, y );

   /* Get endpoints of second arc and enclose the figure, then fill it. */
   _getarcinfo( &xystart, &xyend, &xyfill );
   _moveto( xystart.xcoord, xystart.ycoord );
   _lineto( xyend.xcoord,   xyend.ycoord );
   _floodfill( xyfill.xcoord, xyfill.ycoord, _getcolor() );

   getch();
   _setvideomode( _DEFAULTMODE );
}
