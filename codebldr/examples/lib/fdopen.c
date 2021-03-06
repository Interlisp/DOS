/* FDOPEN.C: This program opens a file using low-level I/O, then uses
 * fdopen to switch to stream access. It counts the lines in the file.
 */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>

void main()
{
   FILE *stream;
   int  fh, count = 0;
   char inbuf[128];

   /* Open a file handle. */
   if( (fh = open( "fdopen.c", O_RDONLY )) == -1 )
      exit( 1 );

   /* Change handle access to stream access. */
   if( (stream = fdopen( fh, "r" )) == NULL )
      exit( 1 );

   while( fgets( inbuf, 128, stream ) != NULL )
      count++;

   /* After fdopen, close with fclose, not close. */
   fclose( stream );

   printf( "Lines in file: %d\n", count );
}
