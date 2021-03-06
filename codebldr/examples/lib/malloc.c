/* MALLOC.C: This program allocates memory with malloc, then frees
 * the memory with free.
 */

#include <stdlib.h>         /* Definition of _MAX_PATH */
#include <stdio.h>
#include <malloc.h>

void main()
{
   char *string;

   /* Allocate space for a path name */
   string = malloc( _MAX_PATH );
   if( string == NULL )
      printf( "Insufficient memory available\n" );
   else
      printf( "Memory space allocated for path name\n" );
   free( string );
   printf( "Memory freed\n" );
}
