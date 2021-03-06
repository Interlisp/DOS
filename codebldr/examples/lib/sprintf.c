/* SPRINTF.C: This program uses sprintf to format various data and
 * place them in the string named buffer.
 */

#include <stdio.h>

void main()
{
   char  buffer[200], s[] = "computer", c = 'l';
   int   i = 35, j;
   float fp = 1.7320534;

   /* Format and print various data: */
   j  = sprintf( buffer,     "\tString:    %s\n", s );
   j += sprintf( buffer + j, "\tCharacter: %c\n", c );
   j += sprintf( buffer + j, "\tInteger:   %d\n", i );
   j += sprintf( buffer + j, "\tReal:      %f\n", fp );

   printf( "Output:\n%s\ncharacter count = %d\n", buffer, j );
}
