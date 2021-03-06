/* STRREV.C: This program checks an input string to see whether it is a
 * palindrome: that is, whether it reads the same forward and backward.
 */

#include <string.h>
#include <stdio.h>

void main()
{
   char string[100];
   int result;

   printf( "Input a string and I will tell you if it is a palindrome:\n" );
   gets( string );

   /* Reverse string and compare (ignore case): */
   result = strcmpi( string, strrev( strdup( string ) ) );
   if( result == 0 )
      printf( "The string \"%s\" is a palindrome\n\n", string );
   else
      printf( "The string \"%s\" is not a palindrome\n\n", string );
}
