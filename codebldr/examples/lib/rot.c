/* ROT.C: This program uses _rotr and _rotl with different shift
 * values to rotate an integer.
 */

#include <stdlib.h>
#include <stdio.h>

void main()
{
   unsigned val = 0x0000fd93;

   printf( "0x%8.8x rotated left three times is 0x%8.8x\n",
           val, _rotl( val, 3 ) );
   printf( "0x%8.8x rotated right four times is 0x%8.8x\n",
           val, _rotr( val, 4 ) );
}
