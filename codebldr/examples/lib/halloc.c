/* HALLOC.C: This program uses halloc to allocate space for 30,000 long
 * integers, then uses hfree to deallocate the memory.
 */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

void main()
{
   long _huge *hbuf;

   /* Allocate huge buffer */
   hbuf = (long _huge *)halloc( 30000L, sizeof( long ) );
   if ( hbuf == NULL )
      printf( "Insufficient memory available\n" );
   else
      printf( "Memory successfully allocated\n" );

   /* Free huge buffer */
   hfree( hbuf );
}
