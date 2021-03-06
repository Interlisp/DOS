/* DCREAT.C: This program creates a file using the _dos_creat function. The
 * program cannot create a new file using the _dos_creatnew function
 * because it already exists.
 */

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

void main()
{
   int fh1, fh2;
   int result;

   if( _dos_creat( "data", _A_NORMAL, &fh1 ) != 0 )
      printf( "Couldn't create data file\n" );
   else
   {
      printf( "Created data file.\n" );

      /* If _dos_creat is successful, the _dos_creatnew call
       * will fail since the file exists
       */
      if( _dos_creatnew( "data", _A_RDONLY, &fh2 ) != 0 )
         printf( "Couldn't create data file\n" );
      else
      {
         printf( "Created data file.\n" );
         _dos_close( fh2 );
      }
      _dos_close( fh1 );
   }
}
