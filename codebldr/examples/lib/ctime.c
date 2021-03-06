/* CTIME.C: This program gets the current time in time_t form, then uses
 * ctime to display the time in string form.
 */

#include <time.h>
#include <stdio.h>

void main()
{
   time_t ltime;

   time( &ltime );
   printf( "The time is %s\n", ctime( &ltime ) );
}
