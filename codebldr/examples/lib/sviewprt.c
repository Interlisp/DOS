/* SVIEWPRT.C: This program sets a viewport and then draws a rectangle
 * around it and an ellipse in it.
 */

#include <conio.h>
#include <stdlib.h>
#include <graph.h>

void main()
{
   /* Find a valid graphics mode. */
   if( !_setvideomode( _MAXRESMODE ) )
      exit( 1 );

   _setviewport( 100, 100, 200, 200 );
   _rectangle( _GBORDER, 0, 0, 100, 100 );
   _ellipse( _GFILLINTERIOR, 10, 10, 90, 90 );

   getch();
   _setvideomode( _DEFAULTMODE );
}
