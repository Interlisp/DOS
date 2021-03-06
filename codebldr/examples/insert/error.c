
/******************************************************************************
*                     Copyright 1991 Intel Corporation.                       *
*       This source is intended for your benefit in developing applications   *
*       using the Intel 386/486 C Code Builder(TM) Kit.  Intel hereby grants  *
*       you permission to modify and incorporate it as needed.                *
******************************************************************************/

/******************************************************************************
Name:
   error -- error handling routines.
Synopsis:
   sys_warning   (format, text, ...)
   warning       (format, text, ...)
   sys_error     (format, text, ...)
   error         (format, text, ...)
******************************************************************************/

#include <stdio.h>
#include <errno.h>
#include "config.h"

/*********************************************************************
<sys_warning>
        * Print error message & errno, then continue.
*********************************************************************/

sys_warning (char *s1, char *s2, char *s3, char *s4, char *s5)
{
    int code = errno;
    if (PROGRAM_NAME) fprintf (stderr, "\t** %s: ", PROGRAM_NAME);
    fprintf (stderr, s1, s2, s3, s4, s5);
    if (0 < code)  fprintf (stderr, " (runtime error #%d)", code);
    fprintf (stderr, "\n");
    fflush  (stderr);
}

/*********************************************************************
<warning>
        * Print error message, then continue.
*********************************************************************/

warning (char *s1, char *s2, char *s3, char *s4, char *s5)
{
    errno = 0;
    sys_warning (s1, s2, s3, s4, s5);
    return;
}

/*********************************************************************
<sys_error>
        * Print error message & errno, then die.
*********************************************************************/

sys_error (char *s1, char *s2, char *s3, char *s4, char *s5)
{
    sys_warning (s1, s2, s3, s4, s5);
    exit (1);
}

/*********************************************************************
<error>
        * Print error message, then die.
*********************************************************************/

error (char *s1, char *s2, char *s3, char *s4, char *s5)
{
    errno = 0;
    sys_warning (s1, s2, s3, s4, s5);
    exit (1);
}
