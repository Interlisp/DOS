/* CNTRL87.C: This program uses _control87 to output the control word,
 * set the precision to 24 bits, and reset the status to the default.
 */

#include <stdio.h>
#include <float.h>

void main()
{
   double a = 0.1;

   /* Show original control word and do calculation. */
   printf( "Original: 0x%.4x\n", _control87( 0, 0 ) );
   printf( "%1.1f * %1.1f = %.15e\n", a, a, a * a );

   /* Set precision to 24 bits and recalculate. */
   printf( "24-bit:   0x%.4x\n", _control87( PC_24, MCW_PC ) );
   printf( "%1.1f * %1.1f = %.15e\n", a, a, a * a );

   /* Restore to default and recalculate. */
   printf( "Default:  0x%.4x\n", _control87( CW_DEFAULT, 0xffff ) );
   printf( "%1.1f * %1.1f = %.15e\n", a, a, a * a );
}
