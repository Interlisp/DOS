/* PUTW.C: This program uses putw to write a word to a stream,
 * then performs an error check.
 */

#include <stdio.h>
#include <stdlib.h>

void main()
{
   FILE *stream;
   unsigned u;

   if( (stream = fopen( "data.out", "wb" )) == NULL )
      exit( 1 );
   for( u = 0; u < 10; u++ )
   {
      putw( u + 0x3a3a2132, stdout );
      putw( u + 0x3a3a2132, stream );   /* Write word to stream. */
      if( ferror( stream ) )        /* Make error check. */
      {
         printf( "putw failed" );
         clearerr( stream );
         exit( 1 );
      }
   }
   printf( "\nWrote ten words\n" );
   fclose( stream );
}
