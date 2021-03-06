/* STRSPN.C: This program uses strspn to determine the length of
 * the segment in the string "cabbage" consisting of a's, b's, and c's.
 * In other words, it finds the first non-abc letter.
 */

#include <string.h>
#include <stdio.h>

void main()
{
   char string[] = "cabbage";
   int  result;

   result = strspn( string, "abc" );
   printf( "The portion of '%s' containing only a, b, or c "
           "is %d bytes long\n", string, result );
}
