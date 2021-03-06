/* FILENO.C: This program uses fileno to obtain the file handle for
 * some standard C streams.
 */

#include <stdio.h>

void main()
{
   printf( "The file handle for stdin is %d\n", fileno( stdin ) );
   printf( "The file handle for stdout is %d\n", fileno( stdout ) );
   printf( "The file handle for stderr is %d\n", fileno( stderr ) );
}
