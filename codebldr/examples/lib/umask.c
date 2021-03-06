/* UMASK.C: This program uses umask to set the file-permission mask so
 * that all future files will be created as read-only files. It also
 * displays the old mask.
 */

#include <sys\types.h>
#include <sys\stat.h>
#include <io.h>
#include <stdio.h>

void main()
{
   int oldmask;

   /* Create read-only files: */
   oldmask = umask( S_IWRITE );
   printf( "Oldmask = 0x%.4x\n", oldmask );
}
