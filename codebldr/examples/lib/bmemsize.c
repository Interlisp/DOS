/* BMEMSIZE.C: This program displays the amount of memory installed. */

#include <bios.h>
#include <stdio.h>

void main()
{
   unsigned memory;

   memory = _bios_memsize();
   printf ( "The amount of memory installed is: %dK\n", memory );
}
