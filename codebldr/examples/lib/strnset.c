/* STRNSET.C */
#include <string.h>
#include <stdio.h>

void main()
{
   char string[15] = "This is a test";

   /* Set not more than 4 characters of string to be *'s */
   printf( "Before: %s\n", string );
   strnset( string, '*', 4 );
   printf( "After:  %s\n", string );
}
