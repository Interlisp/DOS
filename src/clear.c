/* This pgm resets the DOS screen to something readable -jarl */

#include <conio.h>
#include <stddef.h>
#include <stdlib.h>
#include <graph.h>

void main()
{
    exit( !_setvideomode( _DEFAULTMODE ) );
}
