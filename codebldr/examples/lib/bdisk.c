/* BDISK.C: This program first attempts to verify a disk by using an
 * invalid disk head number. After printing the return value error code,
 * the program verifies the disk by using a valid disk head code.
 */

#include <conio.h>
#include <stdio.h>
#include <bios.h>

void main()
{
   unsigned status = 0;
   struct diskinfo_t disk_info;

   disk_info.drive    = 0;
   disk_info.head     = 10;   /* Invalid head number */
   disk_info.track    = 1;
   disk_info.sector   = 2;
   disk_info.nsectors = 8;

   printf( "Insert disk in drive A: and press any key\n" );
   getch();
   status = _bios_disk( _DISK_VERIFY, &disk_info );
   printf( "Return value: 0x%.4x\n", status );
   if( status & 0xff00 )      /* Error if high byte is 0 */
      printf( "Seek error\n" );
   else
      printf( "No seek error\n" );

   printf( "Press any key\n" );
   getch();
   disk_info.head = 0;        /* Valid head number */
   status = _bios_disk( _DISK_VERIFY, &disk_info );
   printf( "Return value: 0x%.4x\n", status );
   if( status & 0xff00 )      /* Error if high byte is 0 */
      printf( "Seek error\n" );
   else
      printf( "No seek error\n" );
}
