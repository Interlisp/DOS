/* HEAPCHK.C: This program checks the heap for consistency
 * and prints an appropriate message.
 */

#include <malloc.h>
#include <stdio.h>

void main()
{
   int  heapstatus;
   char *buffer;

   /* Allocate and deallocate some memory */
   if( (buffer = (char *)malloc( 100 )) != NULL )
      free( buffer );

   /* Check heap status */
   heapstatus = _heapchk();
   switch( heapstatus )
   {
      case _HEAPOK:
         printf(" OK - heap is fine\n" );
         break;
      case _HEAPEMPTY:
         printf(" OK - heap is empty\n" );
         break;
      case _HEAPBADBEGIN:
         printf( "ERROR - bad start of heap\n" );
         break;
      case _HEAPBADNODE:
         printf( "ERROR - bad node in heap\n" );
         break;
   }
}
