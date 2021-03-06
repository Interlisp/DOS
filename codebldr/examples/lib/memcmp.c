/* MEMCMP.C: This program uses memcmp to compare the strings named
 * first and second. If the first 19 bytes of the strings are
 * equal, the program considers the strings to be equal.
 */

#include <string.h>
#include <stdio.h>

void main()
{
   char first[]  = "12345678901234567890";
   char second[] = "12345678901234567891";
   int result;

   printf( "Compare '%.19s' to '%.19s':\n", first, second );
   result = memcmp( first, second, 19 );
   if( result < 0 )
      printf( "First is less than second.\n" );
   else if( result == 0 )
      printf( "First is equal to second.\n" );
   else if( result > 0 )
      printf( "First is greater than second.\n" );
   printf( "Compare '%.20s' to '%.20s':\n", first, second );
   result = memcmp( first, second, 20 );
   if( result < 0 )
      printf( "First is less than second.\n" );
   else if( result == 0 )
      printf( "First is equal to second.\n" );
   else if( result > 0 )
      printf( "First is greater than second.\n" );
}
