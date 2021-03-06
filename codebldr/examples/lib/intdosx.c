/* INTDOSX.C */
#include <dos.h>
#include <stdio.h>

char _far *buffer = "Dollar-sign terminated string\n\r\n\r$";

void main()
{
   union  REGS inregs, outregs;
   struct SREGS segregs;

   /* Print a $-terminated string on the screen using DOS function 0x09. */
   inregs.h.ah = 0x9;
   inregs.x.dx = FP_OFF( buffer );
   segregs.ds  = FP_SEG( buffer );
   intdosx( &inregs, &outregs, &segregs );
}
