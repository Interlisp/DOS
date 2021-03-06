
/******************************************************************************
*                     Copyright 1991 Intel Corporation.                       *
*       This source is intended for your benefit in developing applications   *
*       using the Intel 386/486 C Code Builder(TM) Kit.  Intel hereby grants  *
*       you permission to modify and incorporate it as needed.                *
******************************************************************************/

/******************************************************************************
Name:
   im -- insert error messages from C compiler into source files
Synopsis:
   rse    make      > make.msg
   im     "edlin"   < make.msg    > visit.bat
   visit
******************************************************************************/

#include <stdio.h>
#include <process.h>
#include "config.h"

/** Interfaces to other routines. **/

extern int  is_message          (char*, char**, int*, char**);
extern void save_message        (char*, int, char*);
extern void sort_messages       ();
extern int  get_name            (char**);
extern int  get_number_and_text (int*, char**);

/** Interfaces to local routines. **/

static int move (char *old, char *new);
static int make_obsolete_name (char *buffer, char *name);
static int write_then_read_buffer ();

/** If exp fails, report an error. **/

#define try(exp,msg)  if ((exp)) { sys_error (msg); } ;

/** Save error message text and line number. **/

static struct {
    char *text;
    int  number;
} buffer, msg;

/** Save name and filepointer for source file and a temporary file. **/

static struct {
    char *name;
    FILE *fp;
} src, tmp;

/** Save line read from stdin. **/

static char line[MAX_LINE_LENGTH];

/* Save temporary file name. */

static char tmp_name_buffer[MAX_LINE_LENGTH];

/******************************************************************************
<main>
        * Do main processing.
******************************************************************************/

main (int argc, char *argv[])
{

int nfiles_with_errors;
char *editor;

/** Get parameters. **/

    editor = (argc > 1) ? argv[1] : NULL;

/** Read stdin lines and save the error messages. **/

    while (gets (line)) {
        if (is_message (line, &src.name, &msg.number, &msg.text)) {
            TRACE (("save_message (%s, %d, %s)\n",
                   src.name, msg.number, msg.text));
            save_message (src.name, msg.number, msg.text);
        }
    }

/** Sort saved messages by file name and line number. **/

    sort_messages ();

/** Process sorted messages file-by-file. **/

    nfiles_with_errors = 0;
    while (get_name (&src.name)) {

    /** Echo file name on stdout and stderr. **/

        if (editor) {
            printf ("%s", editor);
            editor = NULL;
        }
        printf (" %s", src.name);
        fprintf (stderr, "\tErrors in %s\n", src.name);
        nfiles_with_errors++;

    /** Open the named file as 'src' and a temporary file as 'tmp'. **/

        sprintf (tmp_name_buffer, TMP_NAME_TEMPLATE);
        tmp.name = tmp_name_buffer;
        try ((src.fp = fopen (src.name, "r")) == NULL, src.name);
        try ((tmp.fp = fopen (tmp.name, "w")) == NULL, tmp.name);

    /** Merge src and message data into tmp. **/

        buffer.number = 0;    /** src line number of text in buffer **/
        buffer.text   = NULL; /** no text yet in buffer **/

        while (get_number_and_text (&msg.number, &msg.text)) {
            TRACE (("number: %d, text: %s\n", msg.number, msg.text));
            while (buffer.number <= msg.number) {
                write_then_read_buffer ();
            }
            fprintf (tmp.fp, "%s\n", msg.text);
            TRACE (("output %d: %s\n", msg.number, msg.text));
        }

        while (write_then_read_buffer ()) { }

        TRACE (("all of file copied.\n"));

    /** Rename tmp as src. **/

        try (fclose (src.fp), src.name);
        try (fclose (tmp.fp), tmp.name);

        if (SHOULD_SAVE_OBSOLETE) {
            make_obsolete_name (line, src.name);
            TRACE (("obsolete name = %s\n", line));
            unlink (line);
            move (src.name, line);
        } else {
           unlink (src.name);
        }

        move (tmp.name, src.name);

    }

/** Exit. **/

    if (nfiles_with_errors > 0) putchar ('\n');
    TRACE (("exit (0)\n"));
    exit (0);

}

/******************************************************************************
<move>
        * Rename old file as new
******************************************************************************/

static int move (char *old, char *new)
{
    TRACE (("move (%s, %s)\n", old, new));
    try (link (old, new), old);
    unlink (old);
}

/******************************************************************************
<make_obsolete>
        * Put into buffer a copy of name with extension replaced with .@@@
******************************************************************************/

static int make_obsolete_name (char *buffer, char *name)
{

char *bp, *dot;

/** Copy name to buffer and point bp at last dot after last slash. **/

    strcpy (buffer, name);
    bp = buffer;
    dot = NULL;
    while (*bp) {
        if (*bp == '/') {
            dot = NULL;
        } else if (*bp == '.') {
            dot = bp;
        }
        bp++;
    }

/** If found such a dot, overwrite with .@@@, else append .@@@ **/

    if (dot) strcpy (dot,  OBSOLETE_SUFFIX);
    else     strcat (line, OBSOLETE_SUFFIX);
}

/******************************************************************************
<write_then_read_buffer>
    * Write info saved in buffer (a global) if buffer.text not null
      to tmp.fp.
    * Then get next line from src.fp into buffer.  Set both text and line
      number.  If reach end-of-file, set text to null and line number
      to very high value.
    * Return 1 iff did not reach end-of-file.
******************************************************************************/

static int write_then_read_buffer ()
{

    if (buffer.text != NULL) {
        TRACE (("output: %s", buffer.text));
        fputs (buffer.text, tmp.fp);
    }

    if (fgets (line, MAX_LINE_LENGTH, src.fp)) {
        TRACE (("input: %s", line));
        buffer.text    = line;
        buffer.number  += 1;
        return 1;
    } else {
        buffer.text    = NULL;
        buffer.number  = 30000;
        return 0;
    }

}
