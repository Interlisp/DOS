/* ECVT.C: This program uses ecvt to convert a floating-point
 * number to a character string.
 */

#include <stdlib.h>
#include <stdio.h>

void main()
{
   int     decimal, sign;
   char    *buffer;
   int     precision = 10;
   double  source = 3.1415926535;

   buffer = ecvt( source, precision, &decimal, &sign );
   printf( "source: %2.10f   buffer: '%s'  decimal: %d   sign: %d\n",
           source, buffer, decimal, sign );
}
