/* EXPAND.C */
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

void main()
{
   char *bufchar;

   printf( "Allocate a 512 element buffer\n" );
   if( (bufchar = (char *)calloc( 512, sizeof( char ) )) == NULL )
      exit( 1 );
   printf( "Allocated %d bytes at %Fp\n",
         _msize( bufchar ), (void _far *)bufchar );

   if( (bufchar = (char *)_expand( bufchar, 1024 )) == NULL )
      printf( "Can't expand" );
   else
      printf( "Expanded block to %d bytes at %Fp\n",
            _msize( bufchar ), (void _far *)bufchar );

   /* Free memory */
   free( bufchar );
   exit( 0 );
}
