/* LSEEK.C: This program first opens a file named LSEEK.C.
 * It then uses lseek to find the beginning of the file,
 * to find the current position in the file, and to find
 * the end of the file.
 */

#include <io.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>


void main()
{
   int fh;
   long pos;               /* Position of file pointer */
   char buffer[10];

   fh = open( "lseek.c", O_RDONLY );

   /* Seek the beginning of the file: */
   pos = lseek( fh, 0L, SEEK_SET );
   if( pos == -1L )
      perror( "lseek to beginning failed" );
   else
      printf( "Position for beginning of file seek = %ld\n", pos );

   /* Move file pointer a little */
   read( fh, buffer, 10 );

   /* Find current position: */
   pos = lseek( fh, 0L, SEEK_CUR );
   if( pos == -1L )
      perror( "lseek to current position failed" );
   else
      printf( "Position for current position seek = %ld\n", pos );

   /* Set the end of the file: */
   pos = lseek( fh, 0L, SEEK_END );
   if( pos == -1L )
      perror( "lseek to end failed" );
   else
      printf( "Position for end of file seek = %ld\n", pos );

   close( fh );
}
