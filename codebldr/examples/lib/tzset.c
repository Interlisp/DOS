/* TZSET.C: This program first sets up the time zone by placing the variable
 * named TZ=EST5 in the environment table. It then uses tzset to set the
 * global variables named daylight, timezone, and tzname.
 */

#include <time.h>
#include <stdlib.h>
#include <stdio.h>

void main()
{

   if( putenv( "TZ=EST5EDT" ) == -1 )
   {
      printf( "Unable to set TZ\n" );
      exit( 1 );
   }
   else
   {
      tzset();
      printf( "daylight = %d\n", daylight );
      printf( "timezone = %ld\n", timezone );
      printf( "tzname[0] = %s\n", tzname[0] );
   }
}
