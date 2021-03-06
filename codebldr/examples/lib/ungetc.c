/* UNGETC.C: This program first converts a character representation of an
 * unsigned integer to an integer. If the program encounters a character
 * that is not a digit, the program uses ungetc to replace it in the stream.
 */

#include <stdio.h>
#include <ctype.h>

void main()
{
   int ch;
   int result = 0;

   printf( "Enter an integer: " );

   /* Read in and convert number: */
   while( ((ch = getchar()) != EOF) && isdigit( ch ) )
      result = result * 10 + ch - '0';       /* Use digit */
   if( ch != EOF )
      ungetc( ch, stdin );                   /* Put non-digit back */
   printf( "Number = %d\nNext character in stream = '%c'\n",
           result, getchar() );
}
