/* FREECT.C: This program determines how much free space is available for
 * integers in the default data segment. Then it allocates space for
 * 1,000 integers and checks the space again, using _freect.
 */

#include <malloc.h>
#include <stdio.h>

void main()
{
   int i;

   /* First report on the free space: */
   printf( "Integers (approximate) available on heap: %u\n\n",
           _freect( sizeof( int ) ) );

   /* Allocate space for 1000 integers: */
   for( i = 0; i < 1000; ++i )
      malloc( sizeof( int ) );

   /* Report again on the free space: */
   printf( "After allocating space for 1000 integers:\n" );
   printf( "Integers (approximate) available on heap: %u\n\n",
           _freect( sizeof( int ) ) );

}
