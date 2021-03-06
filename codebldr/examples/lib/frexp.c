/* FREXP.C: This program calculates frexp( 16.4, &n ), then displays y
 * and n.
 */

#include <math.h>
#include <stdio.h>

void main()
{
   double x, y;
   int n;

   x = 16.4;
   y = frexp( x, &n );
   printf( "frexp( %f, &n ) = %f, n = %d\n", x, y, n );
}
