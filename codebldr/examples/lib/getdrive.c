/* GETDRIVE.C illustrates drive functions including:
 *      _getdrive       _chdrive        _getdcwd
 */

#include <stdio.h>
#include <conio.h>
#include <direct.h>
#include <stdlib.h>

void main()
{
   int ch, drive, curdrive;
   static char path[_MAX_PATH];

   /* Save current drive. */
   curdrive = _getdrive();

   printf( "Available drives are: \n" );

   /* If we can switch to the drive, it exists. */
   for( drive = 1; drive <= 26; drive++ )
      if( !_chdrive( drive ) )
         printf( "%c: ", drive + 'A' - 1 );

   while( 1 )
   {
      printf( "\nType drive letter to check or ESC to quit: " );
      ch = getch();
      if( ch == 27 )
         break;
      if( isalpha( ch ) )
         putch( ch );
      if( _getdcwd( toupper( ch ) - 'A' + 1, path, _MAX_PATH ) != NULL )
         printf( "\nCurrent directory on that drive is %s\n", path );
   }

   /* Restore original drive. This is only necessary for DOS. Under OS/2
    * the current drive of the calling process is always restored.
    */
   _chdrive( curdrive );
   printf( "\n" );
}
