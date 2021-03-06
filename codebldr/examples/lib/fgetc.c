/* FGETC.C: This program uses getc to read the first 80 input characters
 * (or until the end of input) and place them into a string named buffer.
 */

#include <stdio.h>
#include <stdlib.h>

void main()
{
   FILE *stream;
   char buffer[81];
   int  i, ch;

   /* Open file to read line from: */
   if( (stream = fopen( "fgetc.c", "r" )) == NULL )
      exit( 0 );

   /* Read in first 80 characters and place them in "buffer": */
   ch = fgetc( stream );
   for( i=0; (i < 80 ) && ( feof( stream ) == 0 ); i++ )
   {
      buffer[i] = ch;
      ch = fgetc( stream );
   }
   /* Add null to end string */
   buffer[i] = '\0';
   printf( "%s\n", buffer );
   fclose( stream );
}
