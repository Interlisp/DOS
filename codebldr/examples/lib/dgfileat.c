/* DGFILEAT.C: This program creates a file with the specified attributes,
 * then prints this information before changing the file attributes back 
 * to normal.
 */

#include <stdio.h>
#include <dos.h>

void main()
{
   unsigned oldattrib, newattrib;
   int fh;

   /* Get and display file attribute */
   _dos_getfileattr( "DGFILEAT.C", &oldattrib );
   printf( "Attribute: 0x%.4x\n", oldattrib );
   if( ( oldattrib & _A_RDONLY ) != 0 )
      printf( "Read only file\n" );
   else
      printf( "Not a read only file.\n" );

   /* Reset file attribute to normal file */
   _dos_setfileattr( "DGFILEAT.C", _A_RDONLY );
   _dos_getfileattr( "DGFILEAT.C", &newattrib );
   printf( "Attribute: 0x%.4x\n", newattrib );

   /* Restore file attribute */
   _dos_setfileattr( "DGFILEAT.C", oldattrib );
   _dos_getfileattr( "DGFILEAT.C", &newattrib );
   printf( "Attribute: 0x%.4x\n", newattrib );
}
