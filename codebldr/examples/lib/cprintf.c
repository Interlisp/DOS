/* CPRINTF.C: This program displays some variables to the console. */

#include <conio.h>

void main()
{
   int      i = -16, h = 29;
   unsigned u = 62511;
   char     c = 'A';
   char     s[] = "Test";

   /* Note that console output does not translate \n as
    * standard output does. Use \r\n instead.
    */
   cprintf( "%d  %.4x  %u  %c %s\r\n", i, h, u, c, s );

}
