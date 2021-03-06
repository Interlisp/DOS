/* INT86.C: This program uses int86 to call the BIOS video service
 * (INT 10H) to get information about the cursor.
 */

#include <dos.h>
#include <stdio.h>

void main()
{
   union REGS inregs, outregs;

   /* Set up to get cursor information. */
   inregs.h.ah = 3;       /* Get Cursor Position function */
   inregs.h.bh = 0;       /* Page 0 */

   /* Execute video interrupt: */
   int86( 0x10, &inregs, &outregs );

   /* Display results. */
   printf( "Cursor position\n\tRow: %d\n\tColumn: %d\n",
           outregs.h.dh, outregs.h.dl );
   printf( "Cursor shape\n\tStart: %d\n\tEnd: %d\n",
           outregs.h.ch, outregs.h.cl );
}
