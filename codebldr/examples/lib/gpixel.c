/* GPIXEL.C: This program assigns different colors to randomly
 * selected pixels.
 */

#include <conio.h>
#include <stdlib.h>
#include <graph.h>

void main()
{
   short xvar, yvar;
   struct videoconfig vc;

   /* Find a valid graphics mode. */
   if( !_setvideomode( _MAXCOLORMODE ) )
      exit( 1 );
   _getvideoconfig( &vc );

   /* Draw filled ellipse to turn on certain pixels. */
   _ellipse( _GFILLINTERIOR, vc.numxpixels / 6, vc.numypixels / 6,
                             vc.numxpixels / 6 * 5, vc.numypixels / 6 * 5 );

   /* Draw random pixels in random colors... */
   while( !kbhit() )
   {
      /* ...but only if they are already on (inside the ellipse). */
      xvar = rand() % vc.numxpixels;
      yvar = rand() % vc.numypixels;
      if( _getpixel( xvar, yvar ) != 0 )
      {
         _setcolor( rand() % 16 );
         _setpixel( xvar, yvar );
      }
   }

   getch();          /* Throw away the keystroke. */
   _setvideomode( _DEFAULTMODE );
}
