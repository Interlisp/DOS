/* ABS.C: This program computes and displays the absolute values of
 * several numbers.
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

void main()
{
   int    ix = -4, iy;
   long   lx = -41567L, ly;
   double dx = -3.141593, dy;

   iy = abs( ix );
   printf( "The absolute value of %d is %d\n", ix, iy);

   ly = labs( lx );
   printf( "The absolute value of %ld is %ld\n", lx, ly);

   dy = fabs( dx );
   printf( "The absolute value of %f is %f\n", dx, dy );
}
