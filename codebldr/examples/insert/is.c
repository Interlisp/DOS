
/******************************************************************************
*                     Copyright 1991 Intel Corporation.                       *
*       This source is intended for your benefit in developing applications   *
*       using the Intel 386/486 C Code Builder(TM) Kit.  Intel hereby grants  *
*       you permission to modify and incorporate it as needed.                *
******************************************************************************/

/******************************************************************************
Name:
   is -- is this a message?
Synopsis:
   if (is_message (line, &name, &number, &text)) ...
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include "config.h"

/******************************************************************************
<is_message>
    * Examine line; return true if it is an error message.
    * Error message format can be that produced by Intel CodeBuilder
      or various Unix C compilers.
    * Also extract file name, line number, and message text from error
      message and copy results into static areas.
      Point namep, numberp, and textp at these areas.
******************************************************************************/

static char
    buffer[MAX_LINE_LENGTH], name[MAX_LINE_LENGTH], text[MAX_LINE_LENGTH];

static int
    number;

int is_message (char *line, char **namep, int *numberp, char **textp)
{

    int result;

/** Intel CodeBuilder: *** Error at line <number> of <name>: <text>\n **/

    if (sscanf (line,
        "*** Error at line %d of %[^:]: %[^\r\n]",
        &number, name, text) == 3)
    {
        result = 1;

/** Unix format #1: <name>(<number>): <text>\n **/

    } else if (sscanf (line,
        "%[^(](%d) : %[^\r\n]",
        name, &number, text) == 3)
    {
        result = 1;

/** Unix format #2: "<name>", line <number> <text>\n **/

    } else if (sscanf (line,
        "\"%[^\"]\", line %d %[^\r\n]",
        name, &number, text) == 3)
    {
        result = 1;

/** Line is not an error message. **/

    } else {
        result = 0;
    }

/** Point to extracted values if line was an error message. **/

    if (result == 1) {

        sprintf (buffer, ERROR_INSERT_TEMPLATE);

        *numberp = number;
        *namep   = name;
        *textp   = buffer;
    }

/** Return **/

    return result;

}
