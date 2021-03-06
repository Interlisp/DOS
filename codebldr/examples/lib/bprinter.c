/* BPRINTER.C: This program checks the status of the printer attached to
 * LPT1 when it is off-line, then initializes the printer.
 */

#include <bios.h>
#include <conio.h>
#include <stdio.h>

#define LPT1 0

void main()
{
   unsigned status;

   printf ( "Place printer off-line and press any key\n" );
   getch();

   status = _bios_printer( _PRINTER_STATUS, LPT1, 0 );
   printf( "Status with printer off-line: 0x%.4x\n\n", status );
   printf( "Put the printer on-line and then\n" );
   printf( "Press any key to initialize printer\n" );
   getch();

   status = _bios_printer( _PRINTER_INIT, LPT1, 0 );
   printf( "Status after printer initialized: 0x%.4x\n", status );
}
