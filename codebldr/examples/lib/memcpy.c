/* MEMCPY.C. Illustrate overlapping copy: memmove handles it
 * correctly; memcpy does not.
 */
#include <memory.h>
#include <string.h>
#include <stdio.h>

char string1[60] = "The quick brown dog jumps over the lazy fox";
char string2[60] = "The quick brown fox jumps over the lazy dog";
/*                           1         2         3         4         5 
 *                  12345678901234567890123456789012345678901234567890 
 */
void main()
{
   printf( "Function:\tmemcpy without overlap\n" );
   printf( "Source:\t\t%s\n", string1 + 40 );
   printf( "Destination:\t%s\n", string1 + 16 );
   memcpy( string1 + 16, string1 + 40, 3 );
   printf( "Result:\t\t%s\n", string1 );
   printf( "Length:\t\t%d characters\n\n", strlen( string1 ) );

   /* Restore string1 to original contents */
   memcpy( string1 + 16, string2 + 40, 3 );

   printf( "Function:\tmemmove with overlap\n" );
   printf( "Source:\t\t%s\n", string2 + 4 );
   printf( "Destination:\t%s\n", string2 + 10 );
   memmove( string2 + 10, string2 + 4, 40 );
   printf( "Result:\t\t%s\n", string2 );
   printf( "Length:\t\t%d characters\n\n", strlen( string2 ) );

   printf( "Function:\tmemcpy with overlap\n" );
   printf( "Source:\t\t%s\n", string1 + 4 );
   printf( "Destination:\t%s\n", string1 + 10 );
   memcpy( string1 + 10, string1 + 4, 40 );
   printf( "Result:\t\t%s\n", string1 );
   printf( "Length:\t\t%d characters\n\n", strlen( string1 ) );

}
