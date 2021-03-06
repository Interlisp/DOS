/* READ.C: This program opens a file named READ.C and tries to read 60,000
 * bytes from that file using read. It then displays the actual
 * number of bytes read from READ.C.
 */

#include <fcntl.h>      /* Needed only for O_RDWR definition */
#include <io.h>
#include <stdlib.h>
#include <stdio.h>

char buffer[60000];

void main()
{
   int fh;
   unsigned int nbytes = 60000, bytesread;

   /* Open file for input: */
   if( (fh = open( "read.c", O_RDONLY )) == -1 )
   {
      perror( "open failed on input file" );
      exit( 1 );
   }

   /* Read in input: */
   if( ( bytesread = read( fh, buffer, nbytes ) ) <= 0 )
      perror( "Problem reading file" );
   else
      printf( "Read %u bytes from file\n", bytesread );

   close( fh );
}
