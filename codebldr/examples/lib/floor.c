/* FLOOR.C: This example displays the largest integers less than or equal
 * to the floating-point values 2.8 and -2.8. It then shows the smallest
 * integers greater than or equal to 2.8 and -2.8.
 */

#include <math.h>
#include <stdio.h>

void main()
{
   double y;

   y = floor( 2.8 );
   printf( "The floor of 2.8 is %f\n", y );
   y = floor( -2.8 );
   printf( "The floor of -2.8 is %f\n", y );

   y = ceil( 2.8 );
   printf( "The ceil of 2.8 is %f\n", y );
   y = ceil( -2.8 );
   printf( "The ceil of -2.8 is %f\n", y );
}
