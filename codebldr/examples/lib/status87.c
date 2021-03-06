/* STATUS87.C: This program creates various floating-point errors and
 * then uses _status87 to display messages indicating these problems.
 */

#include <stdio.h>
#include <float.h>

void main()
{
   double a = 1e-40, b;
   float  x, y;

   printf( "Status = %.4x - clear\n",_status87() );

   /* Assignment into y is inexact & underflows: */
   y = a;
   printf( "Status = %.4x - inexact, underflow\n", _status87() );

   /* y is denormal: */
   b = y;
   printf( "Status = %.4x - inexact underflow, denormal\n", _status87() );

   /* Clear user 8087: */
   _clear87();
}
