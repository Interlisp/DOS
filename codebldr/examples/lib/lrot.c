/* LROT.C */
#include <stdlib.h>
#include <stdio.h>

void main()
{
   unsigned long val = 0x0fac35791;

   printf( "0x%8.8lx rotated left eight times is 0x%8.8lx\n",
           val, _lrotl( val, 8 ) );
   printf( "0x%8.8lx rotated right four times is 0x%8.8lx\n",
           val, _lrotr( val, 4 ) );
}
