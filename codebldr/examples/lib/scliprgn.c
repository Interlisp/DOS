/* SCLIPRGN.C */
#include <stdlib.h>
#include <conio.h>
#include <graph.h>

void main()
{
   /* Find a valid graphics mode. */
   if( !_setvideomode( _MAXRESMODE ) )
      exit( 1 );

   /* Set clip region, then draw and ellipse larger than the region. */
   _setcliprgn( 0, 0, 200, 125 );
   _ellipse( _GFILLINTERIOR, 80, 50, 240, 190 );

   getch();
   _setvideomode( _DEFAULTMODE );
}
