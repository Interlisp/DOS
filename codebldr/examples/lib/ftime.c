/* FTIME.C: This program uses ftime to obtain the current time
 * and then stores this time in timebuffer.
 */

#include <stdio.h>
#include <sys\timeb.h>
#include <time.h>

void main()
{

   struct timeb timebuffer;
   char *timeline;

   ftime( &timebuffer );
   timeline = ctime( & ( timebuffer.time ) );

   printf( "The time is %.19s.%hu %s",
           timeline, timebuffer.millitm, &timeline[20] );
}
