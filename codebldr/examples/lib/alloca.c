/* ALLOCA.C: This program checks the stack space available before
 * and after using the alloca function to allocate space on the stack.
 */

#include <malloc.h>
#include <stdio.h>

#ifdef _INTELC32_
#pragma stackframe ( main )
#endif

void main()
{
   char *buffer;

#ifndef _INTELC32_
   printf( "Bytes available on stack: %u\n", stackavail() );
#endif

   /* Allocate memory for string. */
   buffer = alloca( 120 * sizeof( char ) );
   printf( "Enter a string: " );
   gets( buffer );
   printf( "You entered: %s\n", buffer );

#ifndef _INTELC32_
   printf( "Bytes available on stack: %u\n", stackavail() );
#endif
}
