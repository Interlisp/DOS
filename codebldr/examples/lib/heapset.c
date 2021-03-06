/* HEAPSET.C: This program checks the heap and fills in free entries
 * with the character 'Z'.
 */

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

void main()
{
   int heapstatus;
   char *buffer;

   if( (buffer = malloc( 1 )) == NULL )   /* Make sure heap is initialized */
      exit( 0 );
   heapstatus = _heapset( 'Z' );          /* Fill in free entries */
   switch( heapstatus )
   {
      case _HEAPOK:
         printf( "OK - heap is fine\n" );
         break;
      case _HEAPEMPTY:
         printf( "OK - heap is empty\n" );
         break;
      case _HEAPBADBEGIN:
         printf( "ERROR - bad start of heap\n" );
         break;
      case _HEAPBADNODE:
         printf( "ERROR - bad node in heap\n" );
         break;
   }
   free( buffer );
}
