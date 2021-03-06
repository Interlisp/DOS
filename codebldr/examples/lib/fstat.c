/* FSTAT.C: This program uses fstat to report the size of a file
 * named FSTAT.OUT.
 */

#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void main()
{
   struct stat buf;
   int fh, result;
   char buffer[] = "A line to output";

   fh = open( "fstat.out", O_CREAT | O_WRONLY | O_TRUNC, S_IWRITE | S_IREAD );
   if ( fh == -1 )
      exit( 1 );
   write( fh, buffer, strlen( buffer ) );

   /* Get data associated with "fh": */

   result = fstat( fh, &buf );

   /* Check if statistics are valid: */
   if( result != 0 )
      printf( "Bad file handle\n" );
   else
   {
      printf( "File size     : %ld\n", buf.st_size );
      printf( "Drive number  : %d\n", buf.st_dev );
      printf( "Time modified : %s", ctime( &buf.st_atime ) );
   }
   close( fh );
}
