/* PGCHART.C: This example illustrates presentation-graphics support
 * routines and single-series chart routines, including
 *    _pg_initchart   _pg_defaultchart   _pg_chart   _pg_chartpie
 */

#include <conio.h>
#include <graph.h>
#include <string.h>
#include <stdlib.h>
#ifndef _INTELC32_
#include <pgchart.h>
#endif

#define COUNTRIES 5
float _far value[COUNTRIES] =    { 42.5,    14.3,    35.2,   21.3,   32.6    };
char _far *category[COUNTRIES] = { "USSR",  "France","USA",  "UK",   "Other" };
short _far explode[COUNTRIES] =  { 0,       1,        0,      1,     0       };

void main()
{
#ifdef _INTELC32_
    printf ( "The C Code Builder(TM) Kit does "
             "not support Presentation-Graphics functions" );
#else
   chartenv env;
   short mode = _VRES16COLOR;

   /* Find a valid graphics mode. */
   if( !_setvideomode( _MAXRESMODE ) )
      exit( 1 );

   _pg_initchart();                    /* Initialize chart system. */

   /* Single-series bar chart */
   _pg_defaultchart( &env, _PG_BARCHART, _PG_PLAINBARS );
   strcpy( env.maintitle.title, "Widget Production" );
   _pg_chart( &env, category, value, COUNTRIES );
   getch();
   _clearscreen( _GCLEARSCREEN );

   /* Single-series column chart */
   _pg_defaultchart( &env, _PG_COLUMNCHART, _PG_PLAINBARS );
   strcpy( env.maintitle.title, "Widget Production" );
   _pg_chart( &env, category, value, COUNTRIES );
   getch();
   _clearscreen( _GCLEARSCREEN );

   /* Pie chart */
   _pg_defaultchart( &env, _PG_PIECHART, _PG_PERCENT );
   strcpy( env.maintitle.title, "Widget Production" );
   _pg_chartpie( &env, category, value, explode, COUNTRIES );
   getch();

   _setvideomode( _DEFAULTMODE );
#endif
}
