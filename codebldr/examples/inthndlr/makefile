#------------------------------------------------------------------------------
# Intel 386/486 C Code Builder(TM) Kit
# Copyright 1991 Intel Corporation.  All Rights Reserved.
#
# Make file for interrupt handling example programs (EXAMPLES\INTHNDLR)
#------------------------------------------------------------------------------

all:  ctlcint.exe  selectiv.exe  advselec.exe  chain.exe  \
      int1B.exe    int1C.exe     int23.exe     int24.exe  \
      int2F.exe    int23a.exe    int24a.exe

advselec.exe:
   icc advselec.c _get_ebx.obj

int23a.exe:
   icc -N -m int23a.obj

int24a.exe:
   icc -N -m int24a.obj

### MKMF:  Do not remove this line!  Automatic dependencies follow.

