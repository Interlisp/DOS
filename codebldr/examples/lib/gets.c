/* GETS.C */

#include <stdio.h>

void main()
{
   char line[81];

   printf( "Input a string: " );
   gets( line );
   printf( "The line entered was: %s\n", line );
}
