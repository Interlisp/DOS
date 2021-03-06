/* DISCURS.C: This program changes the cursor shape using _gettextcursor
 * and _settextcursor, and hides the cursor using _displaycursor.
 */

#include <conio.h>
#include <graph.h>

void main()
{
   short oldcursor;
   short newcursor = 0x007;        /* Full block cursor */

   /* Save old cursor shape and make sure cursor is on */
   oldcursor = _gettextcursor();
   _clearscreen( _GCLEARSCREEN );
   _displaycursor( _GCURSORON );
   _outtext( "\nOld cursor shape: " );
   getch();

   /* Change cursor shape. */
   _outtext( "\nNew cursor shape: " );
   _settextcursor( newcursor );
   getch();

   /* Restore original cursor shape. */
   _outtext( "\n" );
   _settextcursor( oldcursor );
}
