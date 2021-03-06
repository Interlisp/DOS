/* GETC.C: This program uses getchar to read a single line of input
 * from stdin, places this input in buffer, then terminates the
 * string before printing it to the screen.
 */

#include <stdio.h>

void main()
{
   char buffer[81];
   int i, ch;

   printf( "Enter a line: " );

   /* Read in single line from "stdin": */
   for( i = 0; (i < 80) &&  ((ch = getchar()) != EOF) && (ch != '\n'); i++ )
      buffer[i] = ch;

   /* Terminate string with null character: */
   buffer[i] = '\0';
   printf( "%s\n", buffer );
}
