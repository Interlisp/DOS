#==============================================================================
# Intel 386/486 C Code Builder(TM) Kit
# Copyright 1991 Intel Corporation.  All Rights Reserved.
#
# Code Builder Make File for Microsoft(reg.) Library Reference Example Programs
#
# This make file is used to build and run each of the Microsoft Library
# Reference example programs.  Just type "make" to build and run each
# example program. Type "make example.exe" to build and execute a specific
# example.
#
# The make file will pause awaiting a keystroke prior to and following the
# execution of each example program to allow inspection of the results.
# Only the .c source files remain after execution of the example programs.
# The .obj, .rex, and .exe files corresponding the the example programs are
# deleted to prevent cluttering of disk space with 20 megabytes of
# executable modules (241 examples!) To suppress deleting of the .obj and
# and, .exe files, include the 'NO_ERASE=1' command line option.
#
#==============================================================================

#------------------------------------------------------------------------------
# Use Non-ANSI compiler flag to allow Microsoft-specific keywords
#------------------------------------------------------------------------------
CFLAGS = -znoansi

#------------------------------------------------------------------------------
# Override the default link rule to include the graphics library
#------------------------------------------------------------------------------
.obj.exe:
        icc $< graphics.lib

#------------------------------------------------------------------------------
# List of example programs
#------------------------------------------------------------------------------
EXAMPLES =  ABORT.EXE     ABS.EXE       ACCESS.EXE    ALLOCA.EXE     \
            ARC.EXE       ASCTIME.EXE   ASINCOS.EXE   ASSERT.EXE     \
            ATAN.EXE      ATEXIT.EXE    ATOF.EXE      BDISK.EXE      \
            BDOS.EXE      BEGTHRD.EXE   BEQUIPLI.EXE  BESSEL.EXE     \
            BHEAPSEG.EXE  BKEYBRD.EXE   BMEMSIZE.EXE  BPRINTER.EXE   \
            BSEARCH.EXE   BSERIALC.EXE  BTIMEOFD.EXE  CABS.EXE       \
            CALLOC.EXE    CGETS.EXE     CHGDIR.EXE    CHMOD.EXE      \
            CHSIZE.EXE    CLEAR87.EXE   CLEARERR.EXE  CLOCK.EXE      \
            CLRSCRN.EXE   CNTRL87.EXE   CPRINTF.EXE   CPUTS.EXE      \
            CREAT.EXE     CSCANF.EXE    CTIME.EXE     CWAIT.EXE      \
            DALOCMEM.EXE  DCREAT.EXE    DFIND.EXE     DGDISKFR.EXE   \
            DGDRIVE.EXE   DGFILEAT.EXE  DGFTIME.EXE   DGTIME.EXE     \
            DIFFTIME.EXE  DISCURS.EXE   DIV.EXE       DOPEN.EXE      \
            DOSEXERR.EXE  DREAD.EXE     DSTIME.EXE    DUP.EXE        \
            DWRITE.EXE    ECVT.EXE      ELLIPSE.EXE   EOF.EXE        \
            EXEC.EXE      EXITER.EXE    EXP.EXE       EXPAND.EXE     \
            FCVT.EXE      FDOPEN.EXE    FEOF.EXE      FFLUSH.EXE     \
            FGETC.EXE     FGETPOS.EXE   FGETS.EXE     FILENO.EXE     \
            FLOODFIL.EXE  FLOOR.EXE     FLUSHALL.EXE  FMOD.EXE       \
            FOPEN.EXE     FPRESET.EXE   FPRINTF.EXE   FPUTC.EXE      \
            FPUTS.EXE     FP_SEG.EXE    FREAD.EXE     FREECT.EXE     \
            FREOPEN.EXE   FREXP.EXE     FSCANF.EXE    FSEEK.EXE      \
            FSOPEN.EXE    FSTAT.EXE     FTELL.EXE     FTIME.EXE      \
            FULLPATH.EXE  GCURPOS.EXE   GCVT.EXE      GETC.EXE       \
            GETCH.EXE     GETCWD.EXE    GETDRIVE.EXE  GETENV.EXE     \
            GETPID.EXE    GETS.EXE      GETW.EXE      GFILLMSK.EXE   \
            GIMAGE.EXE    GLINESTY.EXE  GMTIME.EXE    GPIXEL.EXE     \
            GVIDCFG.EXE   GWRMODE.EXE   HALLOC.EXE    HEAPCHK.EXE    \
            HEAPMIN.EXE   HEAPSET.EXE   HEAPWALK.EXE  HYPOT.EXE      \
            INT86.EXE     INT86X.EXE    INTDOS.EXE    INTDOSX.EXE    \
            ISATTY.EXE    ISFAM.EXE     ITOA.EXE      KBHIT.EXE      \
            LDEXP.EXE     LDIV.EXE      LFIND.EXE     LOCALTIM.EXE   \
            LOCKING.EXE   LOG.EXE       LROT.EXE      LSEEK.EXE      \
            MAKEDIR.EXE   MAKEPATH.EXE  MALLOC.EXE    MATHERR.EXE    \
            MEMAVL.EXE    MEMCCPY.EXE   MEMCHR.EXE    MEMCMP.EXE     \
            MEMCPY.EXE    MEMICMP.EXE   MEMMAX.EXE    MEMSET.EXE     \
            MINMAX.EXE    MKTEMP.EXE    MKTIME.EXE    MODF.EXE       \
            MOVEDATA.EXE  MOVETO.EXE    ONEXIT.EXE    OPEN.EXE       \
            OUTGTXT.EXE   OUTMEM.EXE    OUTP.EXE      OUTTXT.EXE     \
            PAGE.EXE      PERROR.EXE    PGACHART.EXE  PGCHART.EXE    \
            PGGPAL.EXE    PIE.EXE       PIPE.EXE      POLYGON.EXE    \
            POPEN.EXE     POW.EXE       PRINTF.EXE    PUTC.EXE       \
            PUTS.EXE      PUTW.EXE      QSORT.EXE     RAND.EXE       \
            READ.EXE      REALLOC.EXE   RECT.EXE      REMOVE.EXE     \
            RENAMER.EXE   REWIND.EXE    RMPALPAL.EXE  ROT.EXE        \
            SCANF.EXE     SCLIPRGN.EXE  SCRTXWIN.EXE  SEARCHEN.EXE   \
            SEGREAD.EXE   SELPAL.EXE    SETBUF.EXE    SETMODE.EXE    \
            SETVBUF.EXE   SIGNAL.EXE    SINCOS.EXE    SPAWN.EXE      \
            SPRINTF.EXE   SQRT.EXE      SSCANF.EXE    STAT.EXE       \
            STATUS87.EXE  STRCHR.EXE    STRCMP.EXE    STRCPY.EXE     \
            STRCSPN.EXE   STRDUP.EXE    STRLEN.EXE    STRLWR.EXE     \
            STRNCAT.EXE   STRNCMP.EXE   STRNCPY.EXE   STRNSET.EXE    \
            STRPBRK.EXE   STRREV.EXE    STRSET.EXE    STRSPN.EXE     \
            STRSTR.EXE    STRTIME.EXE   STRTOD.EXE    STRTOK.EXE     \
            STXTROWS.EXE  SVIDMODE.EXE  SVIEWPRT.EXE  SVMROWS.EXE    \
            SVORG.EXE     SWAB.EXE      SWINDOW.EXE   SYSTEM.EXE     \
            TAN.EXE       TELL.EXE      TEMPNAM.EXE   TMPFILE.EXE    \
            TOASCII.EXE   TOUPPER.EXE   TZSET.EXE     UMASK.EXE      \
            UNGETC.EXE    UNGETCH.EXE   UNLINK.EXE    UTIME.EXE      \
            VA.EXE        VPRINTF.EXE   WAIT.EXE      WRAPON.EXE     \
            WRITE.EXE

#------------------------------------------------------------------------------
# Default target (Build and demonstrate all example programs)
#------------------------------------------------------------------------------
all_examples: $(EXAMPLES) byebye

#------------------------------------------------------------------------------
# Display end-of-demonstration message
#------------------------------------------------------------------------------
byebye:
   @cls
   @echo Example programs demonstration complete.

#------------------------------------------------------------------------------
# Build and execute an example program
#------------------------------------------------------------------------------
$(EXAMPLES): $*.c
   @cls                                             # Build executable
   @echo Building $*.exe example program...         #  module for example
   @$(CC) $(CFLAGS) $*.c graphics.lib               #  program

   @cls                                             # Wait for user input
   @echo Ready to run $*.exe example program.       #  before executing
   @pause                                           #  example program

   @cls                                             # Execute example
   @-$*.exe                                         #  program

   @echo Example program $*.exe execution complete. # Wait for user input
   @pause                                           #  before proceeding

   @erase $*.rex                                    # Erase .REX file

   !if null(NO_ERASE)                               # Erase object and
      @erase $*.obj                                 #  executable modules
      @erase $*.exe                                 #  if NO_ERASE is not
   !endif                                           #  defined (value of 0)
   @cls                                             #

#------------------------------------------------------------------------------
# Dependency lines
#------------------------------------------------------------------------------
### MKMF:  Do not remove this line!  Automatic dependencies follow.

