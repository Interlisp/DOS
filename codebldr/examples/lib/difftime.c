/* DIFFTIME.C: This program calculates the amount of time needed to
 * do a floating-point multiply 50000 times.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void main()
{
   time_t   start, finish;
   unsigned loop;
   double   result, elapsed_time;

   printf( "This program will do a floating point multiply 50000 times\n" );
   printf( "Working...\n" );

   time( &start );
   for( loop = 0; loop < 50000L; loop++ )
      result = 3.63 * 5.27;
   time( &finish );

   elapsed_time = difftime( finish, start );
   printf( "\nProgram takes %6.2f seconds.\n", elapsed_time );
}
