/* GETPID.C: This program uses getpid to obtain the process ID and
 * then prints the ID.
 */

#include <stdio.h>
#include <process.h>

void main( )
{
   /* If run from DOS, shows different ID for DOS than for DOS shell.
    * If execed or spawned, shows ID of parent.
    */
   printf( "\nProcess id of parent: %d\n", getpid() );
}
