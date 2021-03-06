/* CHSIZE.C: This program uses filelength to report the size of a
 * file before and after modifying it with chsize.
 */

#include <io.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <stdio.h>

void main()
{
   int fh, result;
   unsigned int nbytes = BUFSIZ;

   /* Open a file */
   if( (fh = open( "data", O_RDWR | O_CREAT, S_IREAD | S_IWRITE )) != -1 )
   {
      printf( "File length before: %ld\n", filelength( fh ) );
      if( chsize( fh, 329678 ) == 0 )
         printf( "Size successfully changed\n" );
      else
         printf( "Problem in changing the size\n" );
      printf( "File length after:  %ld\n", filelength( fh ) );
      close( fh );
   }
}
