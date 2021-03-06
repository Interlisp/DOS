/* OUTGTXT.C illustrates font output using functions:
 *   _registerfonts        _setfont            _outgtext
 *   _unregisterfonts      _getfontinfo        _getgtextextent
 *   _setgtextvector
 */

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <graph.h>

#define NFONTS 6

unsigned char *face[NFONTS] =
{
    "Courier", "Helvetica", "Times Roman", "Modern", "Script", "Roman"
};
unsigned char *options[NFONTS] =
{
    "courier", "helv", "tms rmn", "modern", "script", "roman"
};

void main()
{
    unsigned char list[20];
    char fondir[_MAX_PATH];
    struct videoconfig vc;
    struct _fontinfo fi;
    short fontnum, x, y;

    /* Read header info from all .FON files in current or given directory. */
    if( _registerfonts( "*.FON" ) <= 0 )
    {
        _outtext( "Enter full path where .FON files are located: " );
        gets( fondir );
        strcat( fondir, "\\*.FON" );
        if( _registerfonts( fondir ) <= 0 )
        {
            _outtext( "Error: can't register fonts" );
            exit( 1 );
        }
    }

    /* Set highest available graphics mode and get configuration. */
    if( !_setvideomode( _MAXRESMODE ) )
        exit( 1 );
    _getvideoconfig( &vc );

    /* Display each font name centered on screen. */
    for( fontnum = 0; fontnum < NFONTS; fontnum++ )
    {
        /* Build options string. */
        strcat( strcat( strcpy( list, "t'" ), options[fontnum] ), "'");
        strcat( list, "h30w24b" );

        _clearscreen( _GCLEARSCREEN );
        if( _setfont( list ) >= 0 )
        {
            /* Use length of text and height of font to center text. */
            x = (vc.numxpixels / 2) - (_getgtextextent( face[fontnum] ) / 2);
            y = (vc.numypixels / 2) + (_getgtextextent( face[fontnum] ) / 2);
            if( _getfontinfo( &fi ) )
            {
                _outtext( "Error: Can't get font information" );
                break;
            }
            _moveto( x, y );
            if( vc.numcolors > 2 )
                _setcolor( fontnum + 2 );

            /* Rotate and display text. */
            _setgtextvector( 1, 0 );
            _outgtext( face[fontnum] );
            _setgtextvector( 0, 1 );
            _outgtext( face[fontnum] );
            _setgtextvector( -1, 0 );
            _outgtext( face[fontnum] );
            _setgtextvector( 0, -1 );
            _outgtext( face[fontnum] );
        }
        else
        {
            _outtext( "Error: Can't set font: " );
            _outtext( list );
        }
        getch();
    }
    _unregisterfonts();
    _setvideomode( _DEFAULTMODE );
}
