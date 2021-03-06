/* ATAN.C: This program calculates the arctangent of 1 and -1. */

#include <math.h>
#include <stdio.h>
#include <errno.h>

void main()
{
   double x1, x2, y;

   printf( "Enter a real number: " );
   scanf( "%lf", &x1 );
   y = atan( x1 );
   printf( "Arctangent of %f: %f\n", x1, y );
   printf( "Enter a second real number: " );
   scanf( "%lf", &x2 );
   y = atan2( x1, x2 );
   printf( "Arctangent of %f / %f: %f\n", x1, x2, y );
}
