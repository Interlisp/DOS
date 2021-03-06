/* FLOODFIL.C: This program draws a series of nested rectangles in
 * different colors, constantly changing the background color.
 */

#include <conio.h>
#include <stdlib.h>
#include <graph.h>

void main()
{
   int loop;
   int xvar, yvar;

   /* find a valid graphics mode */
   if( !_setvideomode( _MAXCOLORMODE ) )
      exit( 1 );

   for( xvar = 163, loop = 0; xvar < 320; loop++, xvar += 3 )
   {
      _setcolor( loop % 16 );
      yvar = xvar * 5 / 8;
      _rectangle( _GBORDER, 320-xvar, 200-yvar, xvar, yvar );
      _setcolor( rand() % 16 );
      _floodfill( 0, 0, loop % 16 );
   }
   getch();
   _setvideomode( _DEFAULTMODE );
}
