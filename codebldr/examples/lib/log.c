/* LOG.C: This program uses log and log10 to calculate the natural
 * logarithm and the base-10 logarithm of 9,000.
 */

#include <math.h>
#include <stdio.h>

void main()
{
   double x = 9000.0;
   double y;

   y = log( x );
   printf( "log( %.2f ) = %f\n", x, y );
   y = log10( x );
   printf( "log10( %.2f ) = %f\n", x, y );
}
