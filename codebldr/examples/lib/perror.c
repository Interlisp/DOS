/* PERROR.C: This program attempts to open a file named NOSUCHF.ILE.
 * Since this file probably doesn't exist, an error message is displayed.
 * The same message is created using perror, strerror, and _strerror.
 */

#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <io.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void main()
{
   int  fh;

   if( (fh = open( "NOSUCHF.ILE", O_RDONLY )) == -1 )
   {
      /* Three ways to create error message: */
      perror( "perror says open failed" );
      printf( "strerror says open failed: %s\n", strerror( errno ) );
      printf( _strerror( "_strerror says open failed" ) );
   }
   else
   {
      printf( "open succeeded on input file\n" );
      close( fh );
   }
}
