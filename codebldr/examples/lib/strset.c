/* STRSET.C */
#include <string.h>
#include <stdio.h>


void main()
{
   char string[] = "Fill the string with something";

   printf( "Before: %s\n", string );
   strset( string, '*' );
   printf( "After:  %s\n", string );
}
