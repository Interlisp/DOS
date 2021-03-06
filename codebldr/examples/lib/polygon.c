/* POLYGON.C: This program draws a star-shaped polygon. */

#include <conio.h>
#include <stdlib.h>
#include <graph.h>
#include <math.h>
#include <stdlib.h>

#define PI 3.1415

void main()
{
   short  side, radius = 90, x = 0, y = 0;
   double radians;
   struct xycoord polyside[5];
   struct videoconfig vc;

   /* Find a valid graphics mode. */
   if( !_setvideomode( _MAXRESMODE ) )
      exit( 1 );

   _getvideoconfig( &vc );
   _setvieworg( vc.numxpixels / 2, vc. numypixels / 2 );

   /* Calculate points of star every 144 degrees, then connect them. */
   for( side = 0; side < 5; side++ )
   {
       radians = 144 * PI / 180;
       polyside[side].xcoord = x + (short)(cos( side * radians ) * radius);
       polyside[side].ycoord = y + (short)(sin( side * radians ) * radius);
   }
   _polygon( _GFILLINTERIOR, polyside, 5 );

   getch();
   _setvideomode( _DEFAULTMODE );
}
