/* MAKEDIR.C */
#include <direct.h>
#include <stdlib.h>
#include <stdio.h>

void main()
{
   int result;

   if( mkdir( "\\testtmp" ) == 0 )
   {
      printf( "Directory '\\testtmp' was successfully created\n" );
      system( "dir \\testtmp" );
      if( rmdir( "\\testtmp" ) == 0 )
         printf( "Directory '\\testtmp' was successfully removed\n"  );
      else
         printf( "Problem removing directory '\\testtmp'\n" );
   }
   else
      printf( "Problem creating directory '\\testtmp'\n" );
}
