/* INT86X.C: In this program, int86x executes an INT 21H instruction
 * to invoke DOS system call 43H (change file attributes). The program
 * uses int86x because the file, which is referenced with a far pointer,
 * may be in a segment other than the default data segment. Thus, the
 * program must explicitly set the DS register with the SREGS structure.
 */

#include <signal.h>
#include <dos.h>
#include <stdio.h>
#include <process.h>

char _far *filename = "int86x.c";

void main()
{
   union  REGS inregs, outregs;
   struct SREGS segregs;
   int    result;

   inregs.h.ah = 0x43;      /* DOS function to change attributes    */
   inregs.h.al = 0;         /* Subfunction 0 to get attributes)     */
   inregs.x.dx = FP_OFF( filename );   /* DS:DX points to file name */
   segregs.ds  = FP_SEG( filename );
   result = int86x( 0x21, &inregs, &outregs, &segregs );
   if( outregs.x.cflag )
      printf( "Can't get file attributes; error no. %d\n", result);
   else
      printf( "Attribs = 0x%.4x\n", outregs.x.cx );
}
