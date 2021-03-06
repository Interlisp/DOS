/* RMPALPAL.C: This example illustrates functions for assigning
 * color values to color indices. Functions illustrated include:
 *    _remappalette         _remapallpalette
 */

#include <graph.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>

/* Macro for mixing Red, Green, and Blue elements of color */
#define RGB(r,g,b) (((long) ((b) << 8 | (g)) << 8) | (r))

long tmp, pal[256];
void main()
{
   short  red, blue, green;
   short  inc, i, mode, cells, x, y, xinc, yinc;
   char   buf[40];
   struct videoconfig vc;

   /* Make sure all palette numbers are valid. */
   for( i = 0; i < 256; i++ )
      pal[i] = _BLACK;

   /* Loop through each graphics mode that supports palettes. */
   for( mode = _MRES4COLOR; mode <= _MRES256COLOR; mode++ )
   {
      if( mode == _ERESNOCOLOR )
         mode++;
      if( !_setvideomode( mode ) )
         continue;

      /* Set variables for each mode. */
      _getvideoconfig( &vc );
      switch( vc.numcolors )
      {
         case 256:          /* Active bits in this order:           */
            cells = 13;
            inc = 12;       /* ???????? ??bbbbbb ??gggggg ??rrrrrr  */
            break;
         case  16:
            cells = 4;
            if( (vc.mode == _ERESCOLOR) || (vc.mode == _VRES16COLOR) )
               inc = 16;    /* ???????? ??bb???? ??gg???? ??rr????  */
            else
               inc = 32;    /* ???????? ??Bb???? ??Gg???? ??Rr????  */
            break;
         case   4:
            cells = 2;
            inc = 32;       /* ???????? ??Bb???? ??Gg???? ??Rr????  */
            break;
         default:
             continue;
      }
      xinc = vc.numxpixels / cells;
      yinc = vc.numypixels / cells;

      /* Fill palette arrays in BGR order. */
      for( i = 0, blue = 0; blue < 64; blue += inc )
         for( green = 0; green < 64; green += inc )
            for( red = 0; red < 64; red += inc )
            {
               pal[i] = RGB( red, green, blue );
               /* Special case of using 6 bits to represent 16 colors.
                * If both bits are on for any color, intensity is set.
                * If one bit is set for a color, the color is on.
                */
               if( inc == 32 )
                  pal[i + 8] = pal[i] | (pal[i] >> 1);
               i++;
            }

      /* If palettes available, remap all palettes at once. */
      if( !_remapallpalette( pal ) )
      {
         _setvideomode( _DEFAULTMODE );
         _outtext( "Palettes not available with this adapter" );
         exit( 1 );
      }

      /* Draw colored squares. */
      for( i = 0, x = 0; x < ( xinc * cells ); x += xinc )
         for( y = 0; y < ( yinc * cells); y += yinc )
         {
            _setcolor( i++ );
            _rectangle( _GFILLINTERIOR, x, y, x + xinc, y + yinc );
         }

      /* Note that for 256-color mode, not all colors are shown. The number
       * of colors from mixing three base colors can never be the same as
       * the number that can be shown on a two-dimensional grid.
       */
      sprintf( buf, "Mode %d has %d colors", vc.mode, vc.numcolors );
      _setcolor( vc.numcolors / 2 );
      _outtext( buf );
      getch();

      /* Change each palette entry separately in GRB order. */
      for( i = 0, green = 0; green < 64; green += inc )
         for( red = 0; red < 64; red += inc )
            for(blue = 0; blue < 64; blue += inc )
            {
               tmp = RGB( red, green, blue );
               _remappalette( i, tmp );
               if( inc == 32 )
                  _remappalette( i + 8, tmp | (tmp >> 1) );
               i++;
            }
      getch();
   }
   _setvideomode( _DEFAULTMODE );
}
