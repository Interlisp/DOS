/* ACCESS.C: This example uses access to check the file named "data"
 * to see if it exists and if writing is allowed.
 */

#include <io.h>
#include <stdio.h>
#include <stdlib.h>

void main()
{
   /* Check for existence */
   if( (access( "access.c", 0 )) != -1 )
   {
      printf( "File exists\n" );

      /* Check for write permission */
      if( (access( "access.c", 2 )) != -1 )
         printf( "File has write permision\n" );
   }
}
