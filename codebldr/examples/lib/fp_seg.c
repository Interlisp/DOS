/* FP_SEG.C: This program uses FP_SEG and FP_OFF to obtain
 * the segment and offset of the long pointer p.
 */

#include <dos.h>
#include <malloc.h>
#include <stdio.h>


void main()
{
   void _far *p;
   unsigned int seg_val;
   unsigned int off_val;

   p = _fmalloc( 100 );        /* Points pointer at something */

   seg_val = FP_SEG( p );      /* Gets address pointed to */
   off_val = FP_OFF( p );

   printf( "Segment is %.4X; Offset is %.4X\n", seg_val, off_val );
}
