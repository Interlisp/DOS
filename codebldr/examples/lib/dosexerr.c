/* DOSEXERR.C: This program tries to open the file test.dat. If the
 * attempted open operation fails, the program uses dosexterr to display
 * extended error information.
 */

#include <dos.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>

void main()
{
   struct DOSERROR doserror;
   int fd;

   /* Attempt to open a non-existent file */
   if( (fd = open( "NOSUCHF.ILE", O_RDONLY )) == -1 )
   {
      dosexterr( &doserror );
      printf( "Error: %d  Class: %d  Action: %d  Locus: %d\n",
              doserror.exterror, doserror.class,
              doserror.action,   doserror.locus );
   }
   else
   {
      printf( "Open succeeded so no extended information printed\n" );
      close( fd );
   }
}
