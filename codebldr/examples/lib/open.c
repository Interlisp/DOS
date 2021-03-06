/* OPEN.C: This program uses open to open a file named OPEN.C for input
 * and a file named OPEN.OUT for output. The files are then closed.
 */

#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <io.h>
#include <stdio.h>

void main()
{
   int fh1, fh2;

   fh1 = open( "OPEN.C", O_RDONLY );
   if( fh1 == -1 )
      perror( "open failed on input file" );
   else
   {
      printf( "open succeeded on input file\n" );
      close( fh1 );
   }

   fh2 = open( "OPEN.OUT", O_WRONLY | O_CREAT, S_IREAD | S_IWRITE );
   if( fh2 == -1 )
      perror( "open failed on output file" );
   else
   {
      printf( "open succeeded on output file\n" );
      close( fh2 );
   }
}
