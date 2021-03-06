/* ITOA.C: This program converts integers of various sizes to strings
 * in various radixes.
 */

#include <stdlib.h>
#include <stdio.h>

void main()
{
   char buffer[20];
   int  i = 3445;
   long l = -344115L;
   unsigned long ul = 1234567890UL;

   itoa( i, buffer, 10 );
   printf( "String of integer %d (radix 10): %s\n", i, buffer );
   itoa( i, buffer, 16 );
   printf( "String of integer %d (radix 16): 0x%s\n", i, buffer );
   itoa( i, buffer, 2  );
   printf( "String of integer %d (radix 2): %s\n", i, buffer );

   ltoa( l, buffer, 16 );
   printf( "String of long int %ld (radix 16): 0x%s\n", l, buffer );

   ultoa( ul, buffer, 16 );
   printf( "String of unsigned long %lu (radix 16): 0x%s\n", ul, buffer );
}
