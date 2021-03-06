/* STRLWR.C: This program uses strlwr and strupr to create
 * uppercase and lowercase copies of a mixed-case string.
 */

#include <string.h>
#include <stdio.h>

void main()
{
   char string[100] = "The String to End All Strings!";
   char *copy1, *copy2;

   copy1 = strlwr( strdup( string ) );
   copy2 = strupr( strdup( string ) );
   printf( "Mixed: %s\n", string );
   printf( "Lower: %s\n", copy1 );
   printf( "Upper: %s\n", copy2 );
}
