/* DGDISKFR.C: This program displays information about the default drive. */

#include <stdio.h>
#include <dos.h>

void main()
{
   struct diskfree_t drive;

   /* Get information on default disk drive 0 */

   _dos_getdiskfree( 0, &drive );
   printf( "total clusters: %d\n", drive.total_clusters );
   printf( "available clusters: %d\n", drive.avail_clusters );
   printf( "sectors per cluster: %d\n", drive.sectors_per_cluster );
   printf( "bytes per sector: %d\n", drive.bytes_per_sector );
}
