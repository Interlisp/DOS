/* REMOVE.C: This program uses remove to delete REMOVE.OBJ. */

#include <stdio.h>

void main()
{
   if( remove( "remove.obj" ) == -1 )
      perror( "Could not delete 'REMOVE.OBJ'" );
   else
      printf( "Deleted 'REMOVE.OBJ'\n" );
}
