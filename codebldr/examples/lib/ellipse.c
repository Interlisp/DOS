/* ELLIPSE.C: This program draws a simple ellipse. */

#include <conio.h>
#include <stdlib.h>
#include <graph.h>

void main()
{
   /* Find a valid graphics mode. */
   if( !_setvideomode( _MAXRESMODE ) )
      exit( 1 );

   _ellipse( _GFILLINTERIOR, 80, 50, 240, 150 );

   /* Strike any key to clear screen. */
   getch();
   _setvideomode( _DEFAULTMODE );
}
