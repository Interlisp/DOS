/* MOVEDATA.C */
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <malloc.h>

char _far *src = "This is a test.";

void main()
{
   char _far *dest;

   if( (dest = _fmalloc( 80 )) != NULL )
   {
      movedata( FP_SEG( src ),  FP_OFF( src ),
                FP_SEG( dest ), FP_OFF( dest ), _fstrlen( src ) + 1 );
      printf( "The source data at %Fp is '%Fs'\n", src, src );
      printf( "The destination data at %Fp is '%Fs'\n", dest, dest );
      _ffree( dest );
   }
}
