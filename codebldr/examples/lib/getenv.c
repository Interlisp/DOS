/* GETENV.C: This program uses getenv to retrieve the LIB environment
 * variable and then uses putenv to change it to a new value.
 */

#include <stdlib.h>
#include <stdio.h>

main()
{
   char *libvar;

   /* Get the value of the LIB environment variable. */
   libvar = getenv( "LIB" );
   if( libvar != NULL )
      printf( "Original LIB variable is: %s\n", libvar );

   /* Attempt to change path. Note that this only affects the environment
    * variable of the current process. The command processor's environment
    * is not changed.
    */
   putenv( "LIB=c:\\mylib;c:\\yourlib" );

   /* Get new value. */
   libvar = getenv( "LIB" );
   if( libvar != NULL )
      printf( "New LIB variable is: %s\n", libvar );

}
