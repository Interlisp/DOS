/* BTIMEOFD.C: This program gets the current system clock count before and
 * after a "do-nothing" loop and displays the difference.
 */

#include <bios.h>
#include <stdio.h>

void main()
{
   long i, begin_tick, end_tick;

   _bios_timeofday( _TIME_GETCLOCK, &begin_tick );
   printf( "Beginning tick count: %lu\n", begin_tick );
   for( i = 1; i <= 900000; i++ )
      ;
   _bios_timeofday( _TIME_GETCLOCK, &end_tick );
   printf( "Ending tick count:    %lu\n", end_tick );
   printf( "Elapsed ticks:        %lu\n", end_tick - begin_tick );
}
