/* This program places the name of the current directory in the buffer
 * array, then displays the name of the current directory on the screen.
 * Specifying a length of _MAX_DIR leaves room for the longest legal
 * directory name.
 */

#include <direct.h>
#include <stdlib.h>
#include <stdio.h>

void main()
{
   char buffer[_MAX_DIR];

   /* Get the current working directory: */
   if( getcwd( buffer, _MAX_DIR ) == NULL )
      perror( "getcwd error" );
   else
      printf( "%s\n", buffer );
}
