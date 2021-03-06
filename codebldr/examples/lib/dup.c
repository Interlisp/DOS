/* DUP.C: This program uses the variable old to save the original stdout.
 * It then opens a new file named new and forces stdout to refer
 * to it. Finally, it restores stdout to its original state.
 */

#include <io.h>
#include <stdlib.h>
#include <stdio.h>

void main()
{
   int old;
   FILE *new;

   old = dup( 1 );   /* "old" now refers to "stdout" */
                     /* Note:  file handle 1 == "stdout" */
   if( old == -1 )
   {
      perror( "dup( 1 ) failure" );
      exit( 1 );
   }
   write( old, "This goes to stdout first\r\n", 27 );
   if( ( new = fopen( "data", "w" ) ) == NULL )
   {
      puts( "Can't open file 'data'\n" );
      exit( 1 );
   }

   /* stdout now refers to file "data" */
   if( -1 == dup2( fileno( new ), 1 ) )
   {
      perror( "Can't dup2 stdout" );
      exit( 1 );
   }
   puts( "This goes to file 'data'\r\n" );

   /* Flush stdout stream buffer so it goes to correct file */
   fflush( stdout );
   fclose( new );

   /* Restore original stdout */
   dup2( old, 1 );
   puts( "This goes to stdout\n" );
   puts( "The file 'data' contains:" );
   system( "type data" );
}
