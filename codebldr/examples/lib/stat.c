/* STAT.C: This program uses the stat function to report information
 * about the file named STAT.C.
 */

#include <time.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <stdio.h>

void main()
{
   struct stat buf;
   int  fh, result;
   char buffer[] = "A line to output";

   /* Get data associated with "stat.c": */
   result = stat( "stat.c", &buf );

   /* Check if statistics are valid: */
   if( result != 0 )
      perror( "Problem getting information" );
   else
   {
      /* Output some of the statistics: */
      printf( "File size     : %ld\n", buf.st_size );
      printf( "Drive         : %c:\n", buf.st_dev + 'A' );
      printf( "Time modified : %s", ctime( &buf.st_atime ) );
   }
}
