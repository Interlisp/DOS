#------------------------------------------------------------------------------
# Intel 386/486 C Code Builder(TM) Kit
# Copyright 1991 Intel Corporation.  All Rights Reserved.
#
# Make file for MASM 6.0 - C Interface example program
#------------------------------------------------------------------------------
default:        life.exe

life.exe:       life.obj life_asm.obj
        icc life.obj life_asm.obj graphics.lib

life.obj:       life.c
        icc -c life.c

life_asm.obj:   life_asm.asm
        ml -c -Cx life_asm.asm
