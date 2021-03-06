
/******************************************************************************
*                     Copyright 1991 Intel Corporation.                       *
*       This source is intended for your benefit in developing applications   *
*       using the Intel 386/486 C Code Builder(TM) Kit.  Intel hereby grants  *
*       you permission to modify and incorporate it as needed.                *
******************************************************************************/

/******************************************************************************
Name:
   console -- write a message to the console device if stdout not a tty
Synopsis:
   console "message" ... > file
******************************************************************************/

#include <stdlib.h>
#include <graph.h>

main (int argc, char *argv[])
{
    char *new; /* value of environment variable COLOR; new color index */
    long old;  /* old background color index */
    int i;     /* index of which argv[1..argc-1] to write next. */


    if (! isatty (1)) {
        if ((new = getenv ("COLOR")) && *new) old=_setbkcolor (atol (new));
        for (i = 1; i < argc; i++) {
                if (i > 1) _outtext ((unsigned char *)" ");
                _outtext ((unsigned char *)argv[i]);
        }
        /* if (new && *new) _setbkcolor (old); */
        if (i > 1) _outtext ((unsigned char *)"\n");
    }
    exit (0);
}
