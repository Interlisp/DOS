/* nfile.c - maximum number of open files specifier
 * $Version: 1.1 $
 * Copyright 1991 Intel Corporation, ALL RIGHTS RESERVED.
 *
 * This file is used to change the maximum number of files available to a
 * Code Builder process.   By default, 20 file handles are available to
 * each process.  If an application requires more than 20 files open
 * simultaneously, perform the following steps.
 *
 * 1. Change the value of _NFILE_ to the new maximum desired.
 * 2. Compile this module (i.e., icc -c nfile.c).
 * 3. Link the resulting object module with the applcation.
 *    a. Either explicity specify NFILE.OBJ when linking, or
 *    b. Replace the NFILE module in the CDOS3F.LIB library with the
 *       new module.
 *
 * These changes will take effect during the C start-up code to initialize
 * the maximum handle count and allocate the required memory for file info.
 *
 * Another method to change the maximum file handle count is through use
 * of the undocumented _init_handle_count function.  The description follows.
 *
 * Prototype:
 *   int _init_handle_count ( int new_maximum );
 *
 *   new_maximum - Represents the desired new maximum number of file handles
 *                 per process.  This value must be between 20 and 255.
 *                 Also, it must be less than the value specified in the
 *                 CONFIG.SYS line FILES=n.
 *
 *   The value returned is -1 if an error occured, or the new maximum value.
 *   If the value returned is not equal to the value requested, the requested
 *   value could not be satisfied.  The return value of -1 is returned if
 *   the new maximum is not in the range 20 - 255, if an error occurs
 *   allocating the file handle buffer, or if the request to change the file
 *   handle count could not be satisfied.  If an error occurs, termination
 *   of the application is suggested.
 */

#define _NFILE_ 20

int _nfile = _NFILE_;

