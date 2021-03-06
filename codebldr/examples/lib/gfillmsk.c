/* GFILLMSK.C: This program illustrates _getfillmask and _setfillmask. */

#include <conio.h>
#include <stdlib.h>
#include <graph.h>

void ellipsemask( short x1, short y1, short x2, short y2, char _far *newmask );

unsigned char mask1[8] = { 0x43, 0x23, 0x7c, 0xf7, 0x8a, 0x4d, 0x78, 0x39 };
unsigned char mask2[8] = { 0x18, 0xad, 0xc0, 0x79, 0xf6, 0xc4, 0xa8, 0x23 };
char oldmask[8];

void main()
{
   int loop;

   /* Find a valid graphics mode. */
   if( !_setvideomode( _MAXRESMODE ) )
      exit( 1 );

   /* Set first fill mask and draw rectangle. */
   _setfillmask( mask1 );
   _rectangle( _GFILLINTERIOR, 20, 20, 100, 100 );
   getch();

   /* Call routine that saves and restores mask. */
   ellipsemask( 60, 60, 150, 150, mask2 );
   getch();

   /* Back to original mask. */
   _rectangle( _GFILLINTERIOR, 120, 120, 190, 190 );
   getch();

   _setvideomode( _DEFAULTMODE );
}

/* Draw an ellipse with a specified fill mask. */
void ellipsemask( short x1, short y1, short x2, short y2, char _far *newmask )
{
   unsigned char savemask[8];

   _getfillmask( savemask );                    /* Save mask         */
   _setfillmask( newmask );                     /* Set new mask      */
   _ellipse( _GFILLINTERIOR, x1, y1, x2, y2 );  /* Use new mask      */
   _setfillmask( savemask );                    /* Restore original  */
}
