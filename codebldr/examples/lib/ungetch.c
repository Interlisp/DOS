/* UNGETCH.C: In this program, a white-space delimited token is read
 * from the keyboard. When the program encounters a delimiter,
 * it uses ungetch to replace the character in the keyboard buffer.
 */

#include <conio.h>
#include <ctype.h>
#include <stdio.h>

void main()
{
   char buffer[100];
   int count = 0;
   int ch;

   ch = getche();
   while( isspace( ch ) )      /* Skip preceding white space */
      ch = getche();
   while( count < 99 )         /* Gather token */
   {
      if( isspace( ch ) )      /* End of token */
         break;
      buffer[count++] = ch;
      ch = getche();
   }
   ungetch( ch );              /* Put back delimiter. */
   buffer[count] = '\0';       /* Null terminate the token. */
   printf( "\ntoken = %s\n", buffer );
}
