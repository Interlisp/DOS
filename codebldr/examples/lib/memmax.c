/* MEMMAX.C: This program uses _memmax and _nmalloc to allocate
 * the largest block of memory available in the near heap.
 */

#include <stddef.h>
#include <malloc.h>
#include <stdio.h>

void main()
{
   size_t contig;
   char *p;

   /* Determine contiguous memory size */
   contig = _memmax();
   printf( "Largest block of available memory is %u bytes long\n", contig );
   if( contig )
   {
      /* Error with original Microsoft example program !
       * The above call to _memmax() returns the size (in bytes) of the
       * largest contiguous block available.  In the original version, this
       * value was multiplied by the sizeof(int) in _nmalloc() statement
       * below.  The original version appeared to work because this value
       * is truncated to 16-bits which yielded a value somewhat smaller
       * than the maximum.  Furthermore, the value returned from the
       * Microsoft version of _memmax() is larger than the actual amount
       * available by about 520 bytes.  Therfore the actual maximum amount
       * available is only (contig - 520) The value returned from the Code
       * Builder version is the actual amount available for allocation.
       * The new versions include the following changes:
       *
       * p = _nmalloc( contig * sizeof (int) );     Original
       * p = _nmalloc( contig - 520 );              Microsoft
       * p = _nmalloc( contig );                    Code Builder
       */
      #ifdef _INTELC32_
         p = _nmalloc( contig );
      #else
         p = _nmalloc( contig - 520 );
      #endif

      if( p == NULL )
         printf( "Error with malloc (should never occur)\n" );
      else
      {
         printf( "Maximum allocation succeeded\n" );
         _nfree( p );
      }
   }
   else
      printf( "Near heap is already full\n" );
}
