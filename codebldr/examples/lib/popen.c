/* POPEN.C: This program uses _popen and _pclose to receive a stream
 * of text from a child system process.
 */

#include <stdio.h>
#include <stdlib.h>

void main()
{
#ifdef _INTELC32_
   printf ( "OS/2 functions not available on DOS\n" );
#else

   char    buffer[128];
   FILE    *chkdsk;

   /* Run CHKDSK so that it writes its output to a pipe. Open pipe
    * with read text attribute so that we can read it like a text file.
    */
   if( (chkdsk = _popen( "dir po*.c | sort | more", "rt" )) == NULL )
      exit( 1 );

   /* Read pipe until end of file. End of file indicates that CHKDSK
    * closed its standard out (probably meaning it terminated).
    */
   while( !feof( chkdsk ) )
   {
      if( fgets( buffer, 128, chkdsk ) != NULL )
         printf( buffer );
   }

   /* Close pipe and print return value of CHKDSK. */
   printf( "\nChild returned %d\n", _pclose( chkdsk ) );
#endif
}
