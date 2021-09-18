/* OUTTXT.C: This example illustrates text output functions:
 *    _gettextcolor   _getbkcolor   _gettextposition   _outtext
 *    _settextcolor   _setbkcolor   _settextposition
 */

#include <conio.h>
#include <stdio.h>
#include <graph.h>

char buffer [80];

void main()
{

   /* Save original foreground, background, and text position. */
   short blink, fgd, oldfgd;
   long  bgd, oldbgd;
   struct rccoord oldpos;

   /* Save original foreground, background, and text position. */
   oldfgd = _gettextcolor();
   oldbgd = _getbkcolor();
   oldpos = _gettextposition();
   _clearscreen( _GCLEARSCREEN );

   /* First time no blink, second time blinking. */
   for( blink = 0; blink <= 16; blink += 16 )
   {
      /* Loop through 8 background colors. */
      for( bgd = 0; bgd < 8; bgd++ )
      {
         _setbkcolor( bgd );
         _settextposition( (short)bgd + ((blink / 16) * 9) + 3, 1 );
         _settextcolor( 7 );
         sprintf(buffer, "Back: %d Fore:", bgd );
         _outtext( buffer );

         /* Loop through 16 foreground colors. */
         for( fgd = 0; fgd < 16; fgd++ )
         {
            _settextcolor( fgd + blink );
            sprintf( buffer, " %2d ", fgd + blink );
            _outtext( buffer );
         }
      }
   }
   getch();

   /* Restore original foreground, background, and text position. */
   _settextcolor( oldfgd );
   _setbkcolor( oldbgd );
   _clearscreen( _GCLEARSCREEN );
   _settextposition( oldpos.row, oldpos.col );
}
