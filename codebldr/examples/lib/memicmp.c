/* MEMICMP.C: This program uses memicmp to compare the first
 * 29 letters of the strings named first and second without
 * regard to the case of the letters.
 */

#include <memory.h>
#include <stdio.h>
#include <string.h>

void main()
{
   int result;
   char first[]  = "Those Who Will Not Learn from History";
   char second[] = "THOSE WHO WILL NOT LEARN FROM their mistakes";
   /* Note that the 29th character is right here ^ */

   printf( "Compare '%.29s' to '%.29s'\n", first, second );
   result = memicmp( first, second, 29 );
   if( result < 0 )
      printf( "First is less than second.\n" );
   else if( result == 0 )
      printf( "First is equal to second.\n" );
   else if( result > 0 )
      printf( "First is greater than second.\n" );
}
