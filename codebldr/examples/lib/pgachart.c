/* PGACHART.C: This example illustrates presentation-graphics
 * analyze functions.
 * The example uses
 *      _pg_analyzechartms
 * The same principles apply for
 *      _pg_analyzepie        _pg_analyzechart
 *      _pg_analyzescatter    _pg_analyzescatterms
 */

#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <graph.h>
#ifndef _INTELC32_
#include <pgchart.h>
#endif

#define FALSE  0
#define TRUE   1

/* Note data declared as a single-dimension array. The multiseries
 * chart functions expect only one dimension. See _pg_chartms
 * example for alternate method using multidimension array.
 */
#define TEAMS  4
#define MONTHS 3
float _far values[TEAMS * MONTHS] = { .435,   .522,   .671,
                                      .533,   .431,   .590,
                                      .723,   .624,   .488,
                                      .329,   .226,   .401   };
char _far *months[MONTHS] =         { "May",  "June", "July" };
char _far *teams[TEAMS] = { "Reds", "Sox", "Cubs", "Mets" };

void main()
{
#ifdef _INTELC32_
    printf ( "The C Code Builder(TM) Kit does "
             "not support Presentation-Graphics functions" );
#else
   chartenv env;

   /* Find a valid graphics mode. */
   if( !_setvideomode( _MAXRESMODE ) )
      exit( 1 );

   _pg_initchart();                    /* Initialize chart system.   */

   /* Default multiseries bar chart */
   _pg_defaultchart( &env, _PG_BARCHART, _PG_PLAINBARS );
   strcpy( env.maintitle.title, "Little League Records - Default" );
   _pg_chartms( &env, months, values, TEAMS, MONTHS, MONTHS, teams );
   getch();
    _clearscreen( _GCLEARSCREEN );

   /* Analyze multiseries bar chart with autoscale. This sets all
    * default scale values. We want y axis values to be automatic.
    */
   _pg_defaultchart( &env, _PG_BARCHART, _PG_PLAINBARS );
   strcpy( env.maintitle.title, "Little League Records - Customized" );
   env.xaxis.autoscale = TRUE;
   _pg_analyzechartms( &env, months, values, TEAMS, MONTHS, MONTHS, teams );

   /* Now customize some of the x axis values. Then draw the chart.  */
   env.xaxis.autoscale = FALSE;
   env.xaxis.scalemax = 1.0;           /* Make scale show 0.0 to 1.0.   */
   env.xaxis.ticinterval = 0.2;        /* Don't make scale too crowded. */
   env.xaxis.ticdecimals = 3;          /* Show three decimals.          */
   strcpy( env.xaxis.scaletitle.title, "Win/Loss Percentage" );
   _pg_chartms( &env, months, values, TEAMS, MONTHS, MONTHS, teams );
   getch();

   _setvideomode( _DEFAULTMODE );
#endif
}
