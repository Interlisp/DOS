/* FULLPATH.C: This program demonstrates how _fullpath creates a full
 * path from a partial path.
 */

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <direct.h>

char full[_MAX_PATH], part[_MAX_PATH];

void main()
{
    while( 1 )
    {
        printf( "Enter partial path or ENTER to quit: " );
        gets( part );
        if( part[0] == 0 )
            break;

        if( _fullpath( full, part, _MAX_PATH ) != NULL )
            printf( "Full path is: %s\n", full );
        else
            printf( "Invalid path\n" );
    }
}
