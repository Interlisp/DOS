/* MKTIME.C: The example takes a number of days as input and returns
 * the time, the current date, and the specified number of days.
 */

#include <time.h>
#include <stdio.h>

void main()
{
   struct tm when;
   time_t now, result;
   int    days;

   time( &now );
   when = *localtime( &now );
   printf( "Current time is %s\n", asctime( &when ) );
   printf( "How many days to look ahead: " );
   scanf( "%d", &days );

   when.tm_mday = when.tm_mday + days;
   if( (result = mktime( &when )) != (time_t)-1 )
      printf( "In %d days the time will be %s\n",
              days, asctime( &when ) );
   else
      perror( "mktime failed" );
}
