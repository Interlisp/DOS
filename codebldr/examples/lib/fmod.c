/* FMOD.C: This program displays a floating-point remainder. */

#include <math.h>
#include <stdio.h>

void main()
{
   double x = -10.0, y = 3.0, z;

   z = fmod( x, y );
   printf( "The remainder of %.2f / %.2f is %f\n", x, y, z );
}
