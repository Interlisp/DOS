/* UNLINK.C: This program uses unlink to delete UNLINK.OBJ. */

#include <stdio.h>

void main()
{
   if( unlink( "unlink.obj" ) == -1 )
      perror( "Could not delete 'UNLINK.OBJ'" );
   else
      printf( "Deleted 'UNLINK.OBJ'\n" );
}
