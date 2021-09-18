/* GIMAGE.C: This example illustrates animation routines including:
 *          _imagesize     _getimage     _putimage
 */
/*
#include <conio.h>
#include <stddef.h>
#include <stdlib.h>
#include <malloc.h>
#include <graph.h>
#include <dos.h>

short action[5]  = { _GPSET,   _GPRESET, _GXOR,    _GOR,     _GAND     };
char *descrip[5] = {  "PSET  ", "PRESET", "XOR   ", "OR    ", "AND   " };
*/

#include <graph.h>

void main()
{
/*    long  imsize;
    short i, x, y = 30;
    union REGS inregs, outregs;
*/
    _setvideomode( _MAXRESMODE );
    /* 
      inregs.x.ax = 0x4fff;
    inregs.h.dl = 0x01;

    printf ("inregs.x.ax=[%x], inregs.h.dl=[%x]\n", inregs.x.ax, inregs.h.dl);
    int86( 0x10, &inregs, &outregs );
    printf ("outregs.h.ah=[%x], outregs.h.dl=[%x]\n", outregs.h.ah, outregs.h.dl);


    inregs.x.ax = 0x4f02;
    inregs.x.bx = 0x104;

    printf ("inregs.h.ah=[%x], inregs.h.al=[%x]\n", inregs.h.ah, inregs.h.al);
    int86( 0x10, &inregs, &outregs );
    printf ("outregs.h.ah=[%x], outregs.h.al=[%x]\n", outregs.h.ah, outregs.h.al);

    getch();
    exit(0);
    */
}
