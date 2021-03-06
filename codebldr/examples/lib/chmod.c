/* CHMOD.C: This program uses chmod to change the mode of a file to
 * read-only. It then attempts to modify the file.
 */

#include <sys\types.h>
#include <sys\stat.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>

void main()
{
   /* Make file read-only: */
   if( chmod( "CHMOD.C", S_IREAD ) == -1 )
      perror( "File not found\n" );
   else
      printf( "Mode changed to read-only\n" );
   system( "echo /* End of file */ >> CHMOD.C" );

   /* Change back to read/write: */
   if( chmod( "CHMOD.C", S_IWRITE ) == -1 )
      perror( "File not found\n" );
   else
      printf( "Mode changed to read/write\n" );
}
