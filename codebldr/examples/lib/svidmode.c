/* SVIDMODE.C: This program sets a video mode from a string given on the
 * command line.
 */

#include <graph.h>
#include <stdlib.h>
#include <string.h>

short modes[] = { _TEXTBW40,     _TEXTC40,      _TEXTBW80,
                  _TEXTC80,      _MRES4COLOR,   _MRESNOCOLOR,
                  _HRESBW,       _TEXTMONO,     _HERCMONO,
                  _MRES16COLOR,  _HRES16COLOR,  _ERESNOCOLOR,
                  _ERESCOLOR,    _VRES2COLOR,   _VRES16COLOR,
                  _MRES256COLOR, _ORESCOLOR
                };
char *names[] = { "TEXTBW40",    "TEXTC40",     "TEXTBW80",
                  "TEXTC80",     "MRES4COLOR",  "MRESNOCOLOR",
                  "HRESBW",      "TEXTMONO",    "HERCMONO",
                  "MRES16COLOR", "HRES16COLOR", "ERESNOCOLOR",
                  "ERESCOLOR",   "VRES2COLOR",  "VRES16COLOR",
                  "MRES256COLOR","ORESCOLOR"
                 };

void error( char *msg );


void main( int argc, char *argv[] )
{
   short i, num = sizeof( modes ) / sizeof( short );
   struct videoconfig vc;

   if( argc < 2 )
      error( "No argument given" );

   /* If matching name found, change to corresponding mode. */
   for( i = 0; i < num; i++ )
   {
      if( !strcmpi( argv[1], names[i] ) )
      {
         _setvideomode( modes[i] );
         _outtext( "New mode is: " );
         _outtext( names[i] );
         exit( 0 );
      }
   }
   error( "Invalid mode string" );
}

void error( char *msg )
{
   _outtext( msg );
   exit( 1 );
}
