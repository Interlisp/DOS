/* LOCKING.C: This program opens a file with sharing. It lock some
 * bytes before reading them, then unlocks them. Note that the program
 * works correctly only if the following conditions are met:
 *     - The file exists
 *     - The program is run under OS/2, under DOS 3.0 or later
 *       with file sharing installed (SHARE.COM or SHARE.EXE), or 
 *       if a Microsoft Networks compatible network is running
 */

#include <io.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <sys\locking.h>
#include <share.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

void main()
{
   int  fh, numread;
   long pos, result;
   char buffer[40];

   /* Quit if can't open file or DOS version doesn't support sharing. */
   fh = sopen( "locking.c", O_RDWR, SH_DENYNO, S_IREAD | S_IWRITE );
   if( (fh == -1) || (_osmajor < 3) )
      exit( 1 );

   /* Lock some bytes and read them. Then unlock. */
   if( locking( fh, LK_NBLCK, 30L ) != -1 )
   {
      printf( "No one can change these bytes while I'm reading them\n" );
      numread = read( fh, buffer, 30 );
      printf( "%d bytes read: %.30s\n", numread, buffer );
      locking( fh, LK_UNLCK, 30L );
      printf( "Now I'm done. Do what you will with them\n" );
   }
   else
      perror( "Locking failed\n" );

   close( fh );
}
