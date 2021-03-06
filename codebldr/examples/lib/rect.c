/* RECT.C: This program draws a rectangle. */

#include <conio.h>
#include <stdlib.h>
#include <graph.h>

void main()
{
   /* Find a valid graphics mode. */
   if( !_setvideomode( _MAXRESMODE ) )
      exit( 1 );

   _rectangle( _GBORDER, 80, 50, 240, 150 );

   getch();

   _setvideomode( _DEFAULTMODE );
} 
