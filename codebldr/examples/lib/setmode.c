/* SETMODE.C: This program uses setmode to change stdin from text
 * mode to binary mode.
 */

#include <stdio.h>
#include <fcntl.h>
#include <io.h>

void main()
{
   int result;

   /* Set "stdin" to have binary mode: */
   result = setmode( fileno( stdin ), O_BINARY );
   if( result == -1 )
      perror( "Cannot set mode" );
   else
      printf( "'stdin' successfully changed to binary mode\n" );
}
