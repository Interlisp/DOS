/* SVMROWS.C */

#include <stdlib.h>
#include <conio.h>
#include <graph.h>

void main()
{
   struct videoconfig config;

   /* Set 43-line graphics mode if available. */
   if( !_setvideomoderows( _ERESCOLOR, 43 ) )
   {
      _outtext( "EGA or VGA required" );
      exit( 1 );
   }
   _getvideoconfig( &config );

   /* Set logical origin to center and draw a rectangle. */
   _setlogorg( config.numxpixels / 2 - 1, config.numypixels / 2 - 1 );
   _rectangle( _GBORDER, -80, -50, 80, 50 );

   getch();
   _setvideomode( _DEFAULTMODE );
}
