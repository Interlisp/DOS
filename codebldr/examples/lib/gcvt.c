/* GCVT.C: This program converts -3.1415e5 to its string representation. */

#include <stdlib.h>
#include <stdio.h>

void main()
{
   char buffer[50];
   double source = -3.1415e5;

   gcvt( source, 7, buffer );
   printf( "source: %f  buffer: '%s'\n", source, buffer );
}
