/* TAN.C:  This program displays the tangent of pi / 4 and the hyperbolic
 * tangent of the result.
 */

#include <math.h>
#include <stdio.h>

void main()
{
   double pi = 3.1415926535;
   double x, y;

   x = tan( pi / 4 );
   y = tanh( x );
   printf( "tan( %f ) = %f\n", x, y );
   printf( "tanh( %f ) = %f\n", y, x );
}
