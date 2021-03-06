/* SINCOS.C: This program displays the sine, hyperbolic sine, cosine,
 * and hyperbolic cosine of pi / 2.
 */

#include <math.h>
#include <stdio.h>

void main()
{
   double pi = 3.1415926535;
   double x, y;

   x = pi / 2;
   y = sin( x );
   printf( "sin( %f ) = %f\n", x, y );
   y = sinh( x );
   printf( "sinh( %f ) = %f\n",x, y );
   y = cos( x );
   printf( "cos( %f ) = %f\n", x, y );
   y = cosh( x );
   printf( "cosh( %f ) = %f\n",x, y );
}
