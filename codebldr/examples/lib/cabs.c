/* CABS.C: Using cabs, this program calculates the absolute value of
 * a complex number.
 */

#include <math.h>
#include <stdio.h>

void main()
{
   struct complex number = { 3.0, 4.0 };
   double d;

   d = cabs( number );
   printf( "The absolute value of %f + %fi is %f\n",
           number.x, number.y, d );
}
