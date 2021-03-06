/* SPAWN.C: This program accepts a number in the range 1 - 8 from the
 * command line. Based on the number it receives, it executes one of the
 * eight different procedures that spawn the process named child. For
 * some of these procedures, the CHILD.EXE file must be in the
 * same directory; for others, it only has to be in the same path.
 */

#include <stdio.h>
#include <process.h>

char *my_env[] =
{
   "THIS=environment will be",
   "PASSED=to child.exe by the",
   "SPAWNLE=and",
   "SPAWNLPE=and",
   "SPAWNVE=and",
   "SPAWNVPE=functions",
   NULL
};

void main( int argc, char *argv[] )
{
   char *args[4];
   int  result = 0;

   /* Set up parameters to be sent: */
   args[0] = "child";
   args[1] = "spawn??";
   args[2] = "two";
   args[3] = NULL;
   switch (argv[1][0])   /* Based on first letter of argument */
   {
      case '1':
         result = spawnl( P_WAIT, argv[2], argv[2], "spawnl", "two", NULL );
         break;
      case '2':
         result = spawnle( P_WAIT, argv[2], argv[2], "spawnle", "two",
                  NULL, my_env );
         break;
      case '3':
         result = spawnlp( P_WAIT, argv[2], argv[2], "spawnlp", "two", NULL );
         break;
      case '4':
         result = spawnlpe( P_WAIT, argv[2], argv[2], "spawnlpe", "two",
                   NULL, my_env );
         break;
      case '5':
         result = spawnv( P_OVERLAY, argv[2], args );
         break;
      case '6':
         result = spawnve( P_OVERLAY, argv[2], args, my_env );
         break;
      case '7':
         result = spawnvp( P_OVERLAY, argv[2], args );
         break;
      case '8':
         result = spawnvpe( P_OVERLAY, argv[2], args, my_env );
         break;
      default:
         printf( "SYNTAX: SPAWN <1-8> <childprogram>\n" );
         exit( 1 );
   }
   printf( "\n\nReturned from SPAWN! Return value = %d\n", result );
}
