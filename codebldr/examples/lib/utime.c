/* UTIME.C: This program uses utime to set the file-modification time to
 * the current time.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys\types.h>
#include <sys\utime.h>

void main()
{
   /* Show file time before and after. */
   system( "dir utime.c" );
   if( utime( "utime.c", NULL ) == -1 )
      perror( "utime failed\n" );
   else
      printf( "File time modified\n" );
   system( "dir utime.c" );
}
