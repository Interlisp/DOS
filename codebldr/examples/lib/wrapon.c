/* WRAPON.C */

#include <conio.h>
#include <graph.h>

void main()
{
   _wrapon( _GWRAPON );
   while( !kbhit() )
      _outtext( "Wrap on!   " );
   getch();
   _outtext( "\n\n" );

   _wrapon( _GWRAPOFF );
   while( !kbhit() )
      _outtext( "Wrap off!  " );
   getch();
   _outtext( "\n\n" );
}
