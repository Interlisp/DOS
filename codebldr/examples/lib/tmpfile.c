/* TMPFILE.C: This program uses tmpfile to create a temporary file,
 * then deletes this file with rmtmp.
 */

#include <stdio.h>

void main()
{
   FILE *stream;
   char tempstring[] = "String to be written";
   int  i;

   /* Create temporary files. */
   for( i = 1; i <= 10; i++ )
   {
      if( (stream = tmpfile()) == NULL )
         perror( "Could not open new temporary file\n" );
      else
         printf( "Temporary file %d was created\n", i );
   }

   /* Remove temporary file. */
   printf( "%d temporary files deleted\n", rmtmp() );
}
