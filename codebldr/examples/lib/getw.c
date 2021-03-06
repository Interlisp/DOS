/* GETW.C: This program uses getw to read a word from a stream,
 * then performs an error check.
 */

#include <stdio.h>
#include <stdlib.h>

void main()
{
   FILE *stream;
   int i;

   if( (stream = fopen( "getw.c", "rb" )) == NULL )
      printf( "Couldn't open file\n" );
   else
   {
      /* Read a word from the stream: */
      i = getw( stream );

      /* If there is an error... */
      if( ferror( stream ) )
      {
         printf( "getw failed\n" );
         clearerr( stream );
      }
      else
         printf( "First data word in file: 0x%.4x\n", i );
      fclose( stream );
   }
}
