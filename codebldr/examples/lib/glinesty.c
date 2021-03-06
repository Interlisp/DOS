/* GLINESTY.C: This program illustrates _setlinestyle and _getlinestyle. */

#include <conio.h>
#include <stdlib.h>
#include <graph.h>

void zigzag( short x1, short y1, short size );

void main()
{

   /* Find a valid graphics mode. */
   if( !_setvideomode( _MAXCOLORMODE ) )
      exit( 1 );

   /* Set line style and draw rectangle. */
   _setlinestyle( 0x4d );
   _rectangle( _GBORDER, 10, 10, 60, 60 );
   getch();

   /* Draw figure with function that changes and restores line style. */
   zigzag( 100, 100, 90 );
   getch();

   /* Original style reused. */
   _rectangle( _GBORDER, 190, 190, 130, 130 );
   getch();

   _setvideomode( _DEFAULTMODE );
}

/* Draw box with changing line styles. Restore original style. */
void zigzag( short x1, short y1, short size )
{
   short x, y, oldcolor;
   unsigned short oldstyle;
   unsigned short style[16] = { 0x0001, 0x0003, 0x0007, 0x000f,
                                0x001f, 0x003f, 0x007f, 0x00ff,
                                0x01ff, 0x03ff, 0x07ff, 0x0fff,
                                0x1fff, 0x3fff, 0x7fff, 0xffff };

   oldcolor = _getcolor();
   oldstyle = _getlinestyle();            /* Save old line style.         */
   for( x = 3, y = 3; x < size; x += 3, y += 3 )
   {
      _setcolor( x % 16 );
      _setlinestyle( style[x % 16] );     /* Set and use new line styles. */
      _rectangle( _GBORDER, x1 - x, y1 - y, x1 + x, y1 + y );
   }
   _setlinestyle( oldstyle );             /* Restore old line style.      */
   _setcolor( oldcolor );
}
