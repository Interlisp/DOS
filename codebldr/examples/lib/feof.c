/* FEOF.C: This program uses feof to indicate when it reaches the end
 * of the file FEOF.C. It also checks for errors with ferror.
 */

#include <stdio.h>
#include <stdlib.h>

void main()
{
   int  count, total = 0;
   char buffer[100];
   FILE *stream;

   if( (stream = fopen( "feof.c", "r" )) == NULL )
      exit( 1 );

   /* Cycle until end of file reached: */
   while( !feof( stream ) )
   {
      /* Attempt to read in 10 bytes: */
      count = fread( buffer, sizeof( char ), 100, stream );
      if( ferror( stream ) )
      {
         perror( "Read error" );
         break;
      }

      /* Total up actual bytes read */
      total += count;
   }
   printf( "Number of bytes read = %d\n", total );
   fclose( stream );
}
