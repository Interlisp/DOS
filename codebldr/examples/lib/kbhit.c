/* KBHIT.C: This program loops until the user presses a key.
 * If kbhit returns nonzero, a keystroke is waiting in the buffer.
 * The program can call getch or getche to get the keystroke.
 */

#include <conio.h>
#include <stdio.h>

void main()
{
   /* Display message until key is pressed. */
   while( !kbhit() )
      cputs( "Hit me!! " );

   /* Use getch to throw key away. */
   printf( "\nKey struck was '%c'\n", getch() );
   getch();
}
