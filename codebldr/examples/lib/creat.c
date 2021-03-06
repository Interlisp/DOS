/* CREAT.C: This program uses creat to create the file (or truncate the
 * existing file) named data and open it for writing.
 */

#include <sys\types.h>
#include <sys\stat.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>

void main()
{
   int fh;

   fh = creat( "data", S_IREAD | S_IWRITE );
   if( fh == -1 )
      perror( "Couldn't create data file" );
   else
   {
      printf( "Created data file.\n" );
      close( fh );
   }
}
