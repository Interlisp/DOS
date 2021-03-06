/* SEGREAD.C: This program gets the current segment values with segread. */

#include <dos.h>
#include <stdio.h>

void main()
{
   struct   SREGS segregs;
   unsigned cs, ds, es, ss;

   /* Read the segment register values */
   segread( &segregs );
   cs = segregs.cs;
   ds = segregs.ds;
   es = segregs.es;
   ss = segregs.ss;
   printf( "CS = 0x%.4x    DS = 0x%.4x    ES = 0x%.4x    SS = 0x%.4x\n",
           cs, ds, es, ss );
}
