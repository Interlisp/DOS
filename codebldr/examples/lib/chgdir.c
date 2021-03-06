/* CHGDIR.C: This program uses the chdir function to verify that a
 * given directory exists. Under real mode that directory also becomes
 * the current directory. Under protect mode, it is only the default
 * directory for the current process.
 */

#include <direct.h>
#include <stdio.h>
#include <stdlib.h>

void main( int argc, char *argv[] )
{
   if( chdir( argv[1] )   )
      printf( "Unable to locate the directory: %s\n", argv[1] );
   else
      system( "dir *.c" );
}
