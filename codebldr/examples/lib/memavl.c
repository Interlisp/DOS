/* MEMAVL.C: This program uses _memavl to determine the amount of
 * memory available for dynamic allocation. It then uses malloc to
 * allocate space for 5,000 long integers and uses _memavl again to
 * determine the new amount of available memory.
 */

#include <malloc.h>
#include <stdio.h>

void main()
{
   long *longptr;

   printf( "Memory available before _nmalloc = %u\n", _memavl() );
   if( (longptr = _nmalloc( 5000 * sizeof( long ) )) != NULL )
   {
      printf( "Memory available after _nmalloc = %u\n", _memavl() );
      _nfree( longptr );
   }
}
