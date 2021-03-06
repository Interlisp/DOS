/* LDIV.C: This program takes two long integers as command-line
 * arguments and displays the results of the integer division.
 */

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

void main()
{
   long x = 5149627, y = 234879;
   ldiv_t div_result;

   div_result = ldiv( x, y );
   printf( "For %ld / %ld, the quotient is %ld, and the remainder is %ld\n",
           x, y, div_result.quot, div_result.rem );
}
