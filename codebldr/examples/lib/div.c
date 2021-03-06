/* DIV.C: This example takes two integers as command-line arguments and
 * displays the results of the integer division. This program accepts
 * two arguments on the command line following the program name, then
 * calls div to divide the first argument by the second. Finally,
 * it prints the structure members quot and rem.
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void main( int argc, char *argv[] )
{
   int x,y;
   div_t div_result;

   x = atoi( argv[1] );
   y = atoi( argv[2] );

   printf( "x is %d, y is %d\n", x, y );
   div_result = div( x, y );
   printf( "The quotient is %d, and the remainder is %d\n",
           div_result.quot, div_result.rem );
}
