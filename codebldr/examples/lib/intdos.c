/* INTDOS.C: This program uses intdos to invoke DOS system call 2AH
 * (gets the current date).
 */

#include <dos.h>
#include <stdio.h>

void main()
{
   union REGS inregs, outregs;

   inregs.h.ah = 0x2a;           /* DOS Get Date function: */
   intdos( &inregs, &outregs );
   printf( "Date: %d/%d/%d\n", outregs.h.dh, outregs.h.dl, outregs.x.cx );
}
