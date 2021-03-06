/* BSERIALC.C: This program checks the status of serial port COM1. */

#include <bios.h>
#include <stdio.h>

void main()
{
   unsigned com1_status;

   com1_status =  _bios_serialcom( _COM_STATUS, 0, 0 );
   printf ( "COM1 status: 0x%.4x\n", com1_status );
}
