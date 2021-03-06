/* GMTIME.C: This program uses gmtime to convert a long-integer
 * representation of Greenwich mean time to a structure named newtime,
 * then uses asctime to convert this structure to an output string.
 */

#include <time.h>
#include <stdio.h>

void main()
{
   struct tm *newtime;
   long ltime;

   time( &ltime );

   /* Obtain Greenwich mean time: */
   newtime = gmtime( &ltime );
   printf( "Greenwich mean time is %s\n", asctime( newtime ) );
}
