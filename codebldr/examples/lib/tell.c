/* TELL.C:  This program uses tell to tell the file pointer position
 * after a file read.
 */

#include <io.h>
#include <stdio.h>
#include <fcntl.h>

void main()
{
   int  fh;
   long position;
   char buffer[500];

   if( (fh = open( "tell.c", O_RDONLY )) != -1 )
   {
      if( read( fh, buffer, 500 ) > 0 )
         printf( "Current file position is: %d\n", tell( fh ) );

      close( fh );
   }
}
