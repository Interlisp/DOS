/* PIE.C: This program draws a pie-shaped figure. */

#include <stdlib.h>
#include <conio.h>
#include <graph.h>

void main()
{
   /* Find a valid graphics mode. */
   if( !_setvideomode( _MAXRESMODE ) )
      exit( 1 );

   _pie( _GBORDER, 80, 50, 240, 150, 240, 12, 0, 150 );
   getch();

   _setvideomode( _DEFAULTMODE );

} 
