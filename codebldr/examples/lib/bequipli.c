/* BEQUIPLI.C: This program checks for the presence of diskettes. */

#include <bios.h>
#include <stdio.h>

void main()
{
   unsigned equipment;

   equipment = _bios_equiplist();
   printf( "Equipment bits: 0x%.4x\n", equipment );
   if( equipment & 0x1000 )      /* Check for game adapter bit */
      printf( "Game adapter installed\n" );
   else
      printf( "No game adapter installed\n" );
}
