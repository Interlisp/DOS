/* STRTIME.C */
#include <time.h>
#include <stdio.h>

void main()
{
   char dbuffer [9];
   char tbuffer [9];

   _strdate( dbuffer );
   printf( "The current date is %s \n", dbuffer );
   _strtime( tbuffer );
   printf( "The current time is %s \n", tbuffer );

}
