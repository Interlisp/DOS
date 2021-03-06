/* CLEAR87.C: This program creates various floating-point problems,
 * then uses _clear87 to report on these problems.
 */

#include <stdio.h>
#include <float.h>

void main()
{
   double a = 1e-40, b;
   float x, y;

   printf( "Status: %.4x - clear\n", _clear87()  );

   /* Store into y is inexact and underflows: */
   y = a;
   printf( "Status: %.4x - inexact, underflow\n", _clear87() );

   /* y is denormal: */
   b = y;
   printf( "Status: %.4x - denormal\n", _clear87() );
}
