/* STRPBRK.C */
#include <string.h>
#include <stdio.h>

void main()
{
   char string[100] = "The 3 men and 2 boys ate 5 pigs\n";
   char *result;

   /* Return pointer to first 'a' or 'b' in "string"  */
   printf( "1: %s\n", string );
   result = strpbrk( string, "0123456789" );
   printf( "2: %s\n", result++ );
   result = strpbrk( result, "0123456789" );
   printf( "3: %s\n", result++ );
   result = strpbrk( result, "0123456789" );
   printf( "4: %s\n", result );
}
