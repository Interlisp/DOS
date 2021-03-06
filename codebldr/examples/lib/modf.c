/* MODF.C */
#include <math.h>
#include <stdio.h>

void main()
{
   double x, y, n;

   x = -14.87654321;       /* Divide x into its fractional */
   y = modf( x, &n );      /* and integer parts            */

   printf( "For %f, the fraction is %f and the integer is %.f\n", x, y, n );
}
