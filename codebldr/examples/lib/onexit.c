/* ONEXIT.C */
#include <stdlib.h>
#include <stdio.h>

/* Prototypes */
void fn1( void ), fn2( void ), fn3( void ),  fn4( void );

void main()
{
   onexit( fn1 );
   onexit( fn2 );
   onexit( fn3 );
   onexit( fn4 );
   printf( "This is executed first.\n" );
}

void fn1()
{
   printf( "next.\n" );
}

void fn2()
{
   printf( "executed " );
}

void fn3()
{
   printf( "is " );
}

void fn4()
{
   printf( "This " );
}
