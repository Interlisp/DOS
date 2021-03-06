/* SELPAL.C: This program changes the current CGA palette. */

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <graph.h>

long bkcolor[8] = { _BLACK,  _BLUE,     _GREEN,  _CYAN,
                    _RED,    _MAGENTA,  _BROWN,  _WHITE };
char *bkname [] = { "BLACK", "BLUE",    "GREEN", "CYAN",
                    "RED",   "MAGENTA", "BROWN", "WHITE" };
void main()
{
   int i, j, k;

   if ( !_setvideomode( _MRES4COLOR ) )
   {
      printf( "No palettes available" );
      exit( 1 );
   }

   for( i = 0; i < 4; i++ )                    /* Palette loop          */
   {
       _selectpalette( i );
       for( k = 0; k < 8; k++ )                /* Background color loop */
       {
           _clearscreen( _GCLEARSCREEN );
           _setbkcolor( bkcolor[k] );
           _settextposition( 1, 1 );
           printf( "Background: %s\tPalette: %d", bkname[k], i );
           for( j = 1; j < 4; j++ )            /* Foreground color loop */
           {
               _setcolor( j );
               _ellipse( _GFILLINTERIOR, 100, j * 30, 220, 80 + (j * 30) );
           }
           getch();
       }
   }
   _setvideomode( _DEFAULTMODE );
}
