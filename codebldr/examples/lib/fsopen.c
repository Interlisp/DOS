/* FSOPEN.C: This program opens files named "data" and "data2". It uses
 * fclose to close "data" and fcloseall to close all remaining files.
 */

#include <stdio.h>
#include <share.h>

FILE *stream;

void main()
{
   FILE *stream;

   /* Open output file for writing. Using _fsopen allows us to ensure
    * that no one else writes to the file while we are writing to it.
    */
   if( (stream = _fsopen( "outfile", "wt", SH_DENYWR )) != NULL )
   {
      fprintf( stream, "No one else in the network can write "
                       "to this file until we are done.\n" );
      fclose( stream );
   }
   /* Now others can write to the file while we read it. */
   system( "type outfile" );
}
