/* TOASCII.C: This program encodes a string and decodes it using
 * toascii.
 */

#include <string.h>
#include <stdio.h>
#include <ctype.h>


void main()
{
   char *p, string[] = "Encode this string";

   printf( "Original: %s\n", string );

   /* Encode and decode string. The decoding technique will convert
    * WordStar-style strings, which have some high bits set.
    */
   for( p = string; *p; p++ )
       *p = *p | 0x80;
   printf( "Encoded:  %s\n", string );

   for( p = string; *p; p++ )
       *p = toascii( *p );
   printf( "Decoded:  %s\n", string );
}
