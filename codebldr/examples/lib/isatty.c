/* ISATTY.C: This program checks to see whether stdout has been
 * redirected to a file.
 */

#include <stdio.h>
#include <io.h>

void main()
{
   if( isatty( fileno( stdout ) ) )
      printf( "stdout has not been redirected to a file\n" );
   else
      printf( "stdout has been redirected to a file\n");
}
