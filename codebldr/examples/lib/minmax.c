/* MINMAX.C */
#include <stdlib.h>
#include <stdio.h>

void main()
{
   int a = 10;
   int b = 21;

   printf( "The larger of %d and %d is %d\n",  a, b, max( a, b ) );
   printf( "The smaller of %d and %d is %d\n", a, b, min( a, b ) );
}
