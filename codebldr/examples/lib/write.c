/* WRITE.C: This program opens a file for output and uses write to
 * write some bytes to the file.
 */

#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>

char buffer[] = "This is a test of 'write' function";

void main()
{
   int fh;
   unsigned byteswritten;

   if( (fh = open( "write.o", O_RDWR | O_CREAT, S_IREAD | S_IWRITE )) != -1 )
   {
      if( (byteswritten = write( fh, buffer, sizeof( buffer ) )) == -1 )
         perror( "Write failed" );
      else
         printf( "Wrote %u bytes to file\n", byteswritten );

      close( fh );
   }
}
