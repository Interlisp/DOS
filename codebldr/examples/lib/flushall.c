/* FLUSHALL.C: This program uses flushall to flush all open buffers. */

#include <stdio.h>

void main()
{
   int numflushed;

   numflushed = flushall();
   printf( "There were %d streams flushed\n", numflushed );
}
