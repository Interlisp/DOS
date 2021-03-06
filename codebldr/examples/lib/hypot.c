/* HYPOT.C: This program prints the hypotenuse of a right triangle. */

#include <math.h>
#include <stdio.h>

void main()
{
   double x = 3.0, y = 4.0;

   printf( "If a right triangle has sides %2.1f and %2.1f, "
           "its hypotenuse is %2.1f\n", x, y, hypot( x, y ) );
}
