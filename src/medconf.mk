#############################################################################
#	Makefile for MODXCONF:
#                                                                      
#      Copyright 1991 Intel Corporation.
#      This source is intended for your benefit in developing applications
#      using the Intel 386 / 486 C Code Builder (TM) Kit.  Intel
#      hereby grants you permission to modify and incorporate it as needed.
#
#############################################################################
DB=
VERS=XVERSION
.c.obj:
	icc -c $(DB) -D$(VERS) -znoansi $< 

medconf.exe:	medconf.obj
	icc -xnovm -F -xregion=256K $(DB) medconf.obj
