/* FCVT.C: This program converts the constant 3.1415926535 to a string and
 * sets the pointer *buffer to point to that string.
 */

#include <stdlib.h>
#include <stdio.h>

void main()
{
   int  decimal, sign;
   char *buffer;
   double source = 3.1415926535;

   buffer = fcvt( source, 7, &decimal, &sign );
   printf( "source: %2.10f   buffer: '%s'   decimal: %d   sign: %d\n",
           source, buffer, decimal, sign );
}
