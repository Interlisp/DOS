/* CLRSCRN.C */
#include <conio.h>
#include <graph.h>
#include <stdlib.h>

void main()
{
   short xhalf, yhalf, xquar, yquar;
   struct videoconfig vc;

   /* Find a valid graphics mode. */
   if( !_setvideomode( _MAXRESMODE ) )
      exit( 1 );

   _getvideoconfig( &vc );

   xhalf = vc.numxpixels / 2;
   yhalf = vc.numypixels / 2;
   xquar = xhalf / 2;
   yquar = yhalf / 2;

   _setviewport( 0, 0, xhalf - 1, yhalf - 1 );
   _rectangle( _GBORDER, 0,  0, xhalf - 1, yhalf - 1 );
   _ellipse( _GFILLINTERIOR, xquar / 4, yquar / 4,
                       xhalf - (xquar / 4), yhalf - (yquar / 4) );
   getch();
   _clearscreen( _GVIEWPORT );

   getch();
   _setvideomode( _DEFAULTMODE );
}
