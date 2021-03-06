/* GCURPOS.C: This program sets a random current location, then gets that
 * location with _getcurrentposition.
 */

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <graph.h>

char   buffer[255];

void main()
{
   struct videoconfig vc;
   struct xycoord position;

   /* Find a valid graphics mode. */
   if( !_setvideomode( _MAXRESMODE ) )
      exit( 1 );
   _getvideoconfig( &vc );

   /* Move to random location and report that location. */
   _moveto( rand() % vc.numxpixels, rand() % vc.numypixels );
   position = _getcurrentposition();
   sprintf( buffer, "x = %d, y = %d", position.xcoord, position.ycoord );
   _settextposition( 1, 1 );
   _outtext( buffer );

   getch();
   _setvideomode( _DEFAULTMODE );
}
