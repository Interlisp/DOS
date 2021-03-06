/* EXEC.C: This program accepts a number in the range 1 through 8 from the
 * command line. Based on the number it receives, it executes one of the
 * eight different procedures that spawn the process named child. For
 * some of these procedures, the child.exe file must be in the same
 * directory; for others, it need only be in the same path.
 */

#include <stdio.h>
#include <process.h>

char *my_env[] = {
              "THIS=environment will be",
              "PASSED=to child.exe by the",
              "EXECLE=and",
              "EXECLPE=and",
              "EXECVE=and",
              "EXECVPE=functions",
              NULL
              };

void main( int argc, char *argv[] )
{
   char *args[4];
   int result;

   args[0] = "child";     /* Set up parameters to send */
   args[1] = "execv??";
   args[2] = "two";
   args[3] = NULL;

   switch( argv[1][0] )   /* Based on first letter of argument */
   {
      case '1':
         execl( argv[2], argv[2], "execl", "two", NULL );
         break;
      case '2':
         execle( argv[2], argv[2], "execle", "two", NULL, my_env );
         break;
      case '3':
         execlp( argv[2], argv[2], "execlp", "two", NULL );
         break;
      case '4':
         execlpe( argv[2], argv[2], "execlpe", "two", NULL, my_env );
         break;
      case '5':
         execv( argv[2], args );
         break;
      case '6':
         execve( argv[2], args, my_env );
         break;
      case '7':
         execvp( argv[2], args );
         break;
      case '8':
         execvpe( argv[2], args, my_env );
         break;
      default:
         printf( "SYNTAX: EXEC <1-8> <childprogram>\n" );
         exit( 1 );
   }
   printf( "Process was not spawned.\n" );
   printf( "Program 'child' was not found." );
}
