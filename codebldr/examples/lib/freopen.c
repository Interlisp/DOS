/* FREOPEN.C: This program reassigns stdaux to the file
 * named FREOPEN.OUT and writes a line to that file.
 */

#include <stdio.h>
#include <stdlib.h>

FILE *stream;

void main()
{

   /* Reassign "stdaux" to "freopen.out": */
   stream = freopen( "freopen.out", "w", stdaux );

   if( stream == NULL )
      fprintf( stdout, "error on freopen\n" );
   else
   {
      fprintf( stream, "This will go to the file 'freopen.out'\n" );
      fprintf( stdout, "successfully reassigned\n" );
      fclose( stream );
   }
   system( "type freopen.out" );
}
