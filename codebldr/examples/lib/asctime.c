/* ASCTIME.C: This program places the system time in the long integer aclock,
 * translates it into the structure newtime and then converts it to
 * string form for output, using the asctime function.
 */

#include <time.h>
#include <stdio.h>

struct tm *newtime;
time_t aclock;

void main()
{
   time( &aclock );                    /* Get time in seconds. */

   newtime = localtime( &aclock );     /* Convert time to struct tm form. */

   /* Print local time as a string. */
   printf( "The current date and time are: %s\n", asctime( newtime ) );
}
