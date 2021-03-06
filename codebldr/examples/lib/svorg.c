/* SVORG.C: This program sets the view origin to the center of
 * the screen, then draws a rectangle using the new origin.
 */

#include <stdlib.h>
#include <conio.h>
#include <graph.h>

void main()
{
   struct videoconfig config;

   /* Find a valid graphics mode. */
   if( !_setvideomode( _MAXRESMODE ) )
      exit( 1 );
   _getvideoconfig( &config );

   /* Set view origin to the center of the screen. */
   _setvieworg( config.numxpixels / 2, config.numypixels / 2 );
   _rectangle( _GBORDER, -80, -50, 80, 50 );

   getch();
   _setvideomode( _DEFAULTMODE );
}
