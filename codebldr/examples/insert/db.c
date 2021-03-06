
/******************************************************************************
*                     Copyright 1991 Intel Corporation.                       *
*       This source is intended for your benefit in developing applications   *
*       using the Intel 386/486 C Code Builder(TM) Kit.  Intel hereby grants  *
*       you permission to modify and incorporate it as needed.                *
******************************************************************************/

/******************************************************************************
Name:
   db -- manage message database
Synopsis:
   void save_message        (char *, int *, char *);
   void sort_messages       ();
   int  get_name            (char **);
   int  get_number_and_text (int *, char **);
******************************************************************************/

#include <string.h>
#include "config.h"

/** Save file name, line number, and message text. **/

typedef struct {
    char *name;
    int   number;
    char *text;
} msg;

static msg  *data[MAX_NUMBER_MESSAGES];
static int   ndata = 0, nget = 0, overflow = 0;
static char *last_name = "";

/******************************************************************************
<save_message>
    * Save error message parts for later sorting and reading.
******************************************************************************/

save_message (char *name, int number, char *text)
{
    msg *new_msg;

    if (ndata < MAX_NUMBER_MESSAGES) {
        if (strcmp (name, last_name) != 0) {
            last_name = strdup (name);
        }
        new_msg         = (msg *) malloc (sizeof (msg));
        new_msg->name   = last_name;
        new_msg->number = number;
        new_msg->text   = strdup (text);
        data[ndata++]   = new_msg;
    } else if (overflow == 0) {
        warning ("Too many error messages; some messages ignored.");
        overflow = 1;
    }

}

/******************************************************************************
<sort_messages>
    * Sort saved messages by file name, line number.
******************************************************************************/

int compare (msg **xp, msg **yp)
{
    msg *x, *y;
    int order;

    x = *xp;
    y = *yp;
    if ((order = strcmp (x->name, y->name)) != 0) {
        return order;
    } else {
        return x->number - y->number;
    }

}

sort_messages ()
{
    qsort (data, ndata, sizeof (msg *), compare);
}

/******************************************************************************
<get_name>
    * Return true if some saved messages haven't yet been read.
    * Also, point namep at file name of next message.
******************************************************************************/

int get_name (char **namep)
{
    if (nget < ndata) {
        *namep = last_name = data[nget]->name;
        return 1;
    } else {
        return 0;
    }
}

/******************************************************************************
<get_number_and_text>
    * Return true if there are more messages with same file name as that
      returned by last get_name.
    * Also, point numberp and textp at line number and message text from
      next such message.
******************************************************************************/

int get_number_and_text (int *numberp, char **textp)
{

    if (nget < ndata && strcmp (last_name, data[nget]->name) == 0) {
        *numberp = data[nget]->number;
        *textp   = data[nget]->text;
        nget++;
        return 1;
    } else {
        return 0;
    }

}
