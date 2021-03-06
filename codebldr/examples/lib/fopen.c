/* FOPEN.C: This program opens files named "data" and "data2". It uses
 * fclose to close "data" and fcloseall to close all remaining files.
 */

#include <stdio.h>

FILE *stream, *stream2;

void main()
{
   int numclosed;

   /* Open for read (will fail if 'data' does not exist) */
   if( (stream  = fopen( "data", "r" )) == NULL )
      printf( "The file 'data' was not opened\n" );
   else
      printf( "The file 'data' was opened\n" );

   /* Open for write */
   if( (stream2 = fopen( "data2", "w+" )) == NULL )
      printf( "The file 'data2' was not opened\n" );
   else
      printf( "The file 'data2' was opened\n" );

   /* Close stream */
   if( fclose( stream ) )
      printf( "The file 'data' was not closed\n" );

   /* All other files are closed: */
   numclosed = fcloseall( );
   printf( "Number of files closed by fcloseall: %u\n", numclosed );
}
