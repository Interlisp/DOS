/* GETCH.C: This program reads characters from the keyboard until it
 * receives a 'Y' or 'y'.
 */

#include <conio.h>
#include <ctype.h>

void main()
{
   int ch;

   cputs( "Type 'Y' when finished typing keys: " );
   do
   {
      ch = getch();
      ch = toupper( ch );
   } while( ch != 'Y' );

   putch( ch );
   putch( '\r' );    /* Carriage return */
   putch( '\n' );    /* Line feed       */
}
