#------------------------------------------------------------------------------
# Intel 386/486 C Code Builder(TM) Kit
# Copyright 1991 Intel Corporation.  All Rights Reserved.
#
# Make file for Mouse example program
#------------------------------------------------------------------------------

.c.exe:
   $(CC) $(CFLAGS) $(LINKFLAGS) $< MOUSE.LIB

.obj.exe:
   $(CC) $(LINKFLAGS) $< MOUSE.LIB

default: mousetst.exe mt2.exe

mousetst.exe: mousetst.obj

mt2.exe: mt2.obj

### MKMF:  Do not remove this line!  Automatic dependencies follow.

mousetst.obj: mousetst.h

mt2.obj: mousetst.h
