/* FGETS.C: This program uses fgets to display a line from a file on the
 * screen.
 */

#include <stdio.h>

FILE *stream;

void main()
{
   char line[100], *result;

   if( (stream = fopen( "fgets.c", "r" )) != NULL )
   {
      if( fgets( line, 100, stream ) == NULL)
         printf( "fgets error\n" );
      else
         printf( "%s", line);
      fclose( stream );
   }
}
