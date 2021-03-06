/* STXTROWS.C: This program attempts to set the screen height. It returns
 * an errorlevel code of 1 (fail) or 0 (success) that could be tested in
 * a batch file.
 */

#include <graph.h>
#include <stdlib.h>

void main( int argc, char **argv )
{
   short rows;

   if( !(rows = atoi( argv[1] )) )
   {
      _outtext( "\nSyntax: STXTROWS [ 25 | 43 | 50 ]\n" );
      exit( 1 );
   }

   /* Make sure new rows are the same as requested rows. */
   if( _settextrows( rows ) != rows )
   {
      _outtext( "\nInvalid rows\n" );
      exit( 1 );
   }
   else
      exit( 0 );
}
