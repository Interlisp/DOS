/* PGGPAL.C: This example illustrates presentation-graphics palettes
 * and the routines that modify them, including
 *    _pg_getpalette    _pg_resetpalette    _pg_setstyleset
 *    _pg_getstyleset   _pg_resetstyleset   _pg_vlabelchart
 *    _pg_hlabelchart   _pg_setpalette
 */
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <graph.h>
#ifndef _INTELC32_
#include <pgchart.h>
#endif

#define TEAMS  2
#define MONTHS 3
float _far values[TEAMS][MONTHS] = { { .435,   .522,   .671  },
                                     { .533,   .431,   .401  } };
char _far *months[MONTHS] =          { "May", "June", "July" };
char _far *teams[TEAMS] = { "Cubs", "Reds" };


#ifndef _INTELC32_
fillmap fill1 = { 0x99, 0x33, 0x66, 0xcc, 0x99, 0x33, 0x66, 0xcc };
fillmap fill2 = { 0x99, 0xcc, 0x66, 0x33, 0x99, 0xcc, 0x66, 0x33 };
styleset styles;
palettetype pal;
#endif

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

   _pg_initchart();                         /* Initialize chart system.    */

   /* Modify global set of line styles used for borders, grids, and
    * data connectors. Note that this change is used before
    * _pg_defaultchart, which will use the style set.
    */
   _pg_getstyleset( styles );               /* Get styles and modify      */
   styles[1] = 0x5555;                      /*   style 1 (used for        */
   _pg_setstyleset( styles );              /*   borders)--then set new.   */

   _pg_defaultchart( &env, _PG_BARCHART, _PG_PLAINBARS );

   /* Modify palette for data lines, colors, fill patterns, and
    * characters. Note that the line styles are set in the palette, not
    * in the style set, so that only data connectors will be affected.
    */
   _pg_getpalette( pal );                   /* Get default palette         */
   pal[1].plotchar = 16;                    /* Set to ASCII 16 and 17      */
   pal[2].plotchar = 17;
   memcpy( pal[1].fill, fill1, 8 );         /* Copy fill masks to  palette */
   memcpy( pal[2].fill, fill2, 8 );
   pal[1].color = 3;                        /* Change palette colors       */
   pal[2].color = 4;
   pal[1].style = 0xfcfc;                   /* Change palette line styles  */
   pal[2].style = 0x0303;
   _pg_setpalette( pal );                   /* Put modified palette        */

   /* Multiseries bar chart */
   strcpy( env.maintitle.title, "Little League Records - Customized" );
   _pg_chartms( &env, months, (float _far *)values,
             TEAMS, MONTHS, MONTHS, teams );
   getch();
   _clearscreen( _GCLEARSCREEN );

   /* Multiseries line chart */
   _pg_defaultchart( &env, _PG_LINECHART, _PG_POINTANDLINE );
   strcpy( env.maintitle.title, "Little League Records - Customized" );
   _pg_chartms( &env, months, (float _far *)values,
                 TEAMS, MONTHS, MONTHS, teams );

   /* Print labels. */
   _pg_hlabelchart( &env, (short)(env.chartwindow.x2 * .75),
                          (short)(env.chartwindow.y2 * .10),
                          12, "Up and up!" );
   _pg_vlabelchart( &env, (short)(env.chartwindow.x2 * .75),
                          (short)(env.chartwindow.y2 * .45),
                          13, "Sliding down!" );
   getch();
   _clearscreen( _GCLEARSCREEN );

   _pg_resetpalette();                     /* Restore default palette    */
   _pg_resetstyleset();                    /*   and style set.            */

   /* Multiseries bar chart */
   _pg_defaultchart( &env, _PG_BARCHART, _PG_PLAINBARS );
   strcpy( env.maintitle.title, "Little League Records - Default" );
   _pg_chartms( &env, months, (float _far *)values,
                TEAMS, MONTHS, MONTHS, teams );
   getch();
   _clearscreen( _GCLEARSCREEN );

   /* Multiseries line chart */
   _pg_defaultchart( &env, _PG_LINECHART, _PG_POINTANDLINE );
   strcpy( env.maintitle.title, "Little League Records - Default" );
   _pg_chartms( &env, months, (float _far *)values,
                TEAMS, MONTHS, MONTHS, teams );
   getch();

   _setvideomode( _DEFAULTMODE );
#endif
}
