/* EOF.C: This program reads data from a file ten bytes at a time
 * until the end of the file is reached or an error is encountered.
 */

#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

void main()
{
   int  fh, count, total = 0;
   char buf[10];

   if( (fh = open( "eof.c", O_RDONLY )) == - 1 )
      exit( 1 );

   /* Cycle until end of file reached: */
   while( !eof( fh ) )
   {
      /* Attempt to read in 10 bytes: */
      if( (count = read( fh, buf, 10 )) == -1 )
      {
         perror( "Read error" );
         break;
      }

      /* Total up actual bytes read */
      total += count;
   }
   printf( "Number of bytes read = %d\n", total );
   close( fh );
}
