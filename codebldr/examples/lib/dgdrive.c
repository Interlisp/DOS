/* DGDRIVE.C: This program prints the letter of the current drive,
 * changes the default drive to A, then returns the number of disk drives.
 */

#include <stdio.h>
#include <dos.h>

void main()
{
   unsigned olddrive, newdrive;
   unsigned number_of_drives;

   /* Print current default drive information */
   _dos_getdrive( &olddrive );
   printf( "The current drive is: %c\n", 'A' + olddrive - 1 );

   /* Set default drive to be drive A */
   printf( "Changing default drive to A\n");
   _dos_setdrive( 1, &number_of_drives );

   /* Get new default drive information and total number of drives */
   _dos_getdrive( &newdrive );
   printf( "The current drive is: %c\n", 'A' + newdrive - 1 );
   printf( "Number of logical drives: %d\n", number_of_drives );

   /* Restore default drive */
   _dos_setdrive( olddrive, &number_of_drives );
}
