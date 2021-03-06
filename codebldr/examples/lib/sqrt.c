/* SQRT.C: This program calculates a square root. */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void main()
{
   double question = 45.35, answer;

   answer = sqrt( question );
   if( errno == EDOM )
      printf( "Domain error\n" );
   else
      printf( "The square root of %.2f is %.2f\n", question, answer );
}
