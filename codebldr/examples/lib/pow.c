/* POW.C */
#include <math.h>
#include <stdio.h>

void main()
{
   double x = 2.0, y = 3.0, z;

   z = pow( x, y );
   printf( "%.1f to the power of %.1f is %.1f\n", x, y, z );
}
