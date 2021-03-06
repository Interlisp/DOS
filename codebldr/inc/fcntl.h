/* fcntl.h - file control options for the open function
 * $Version: 1.7 $
 * Copyright 1988,89,90,91 Intel Corporation, ALL RIGHTS RESERVED.
 */

#ifndef _fcntlh
#define _fcntlh

#define  O_RDONLY      0x0000   /* Read only                                 */
#define  O_WRONLY      0x0001   /* Write only                                */
#define  O_RDWR        0x0002   /* Read/Write, update mode                   */
#define  O_APPEND      0x0008   /* Append mode                               */

#define  O_NOINHERIT   0x0080   /* File not inherited by child process       */

#define  O_CREAT       0x0100   /* Create and open file                      */
#define  O_TRUNC       0x0200   /* Length is truncated to 0                  */
#define  O_EXCL        0x0400   /* Exclusive open, used with O_CREAT         */

#define  O_NOCTTY      0x1000   /* The open of a terminal device open will   */
                                /* not become the controling terminal for    */
                                /* the process                               */

#define  O_NONBLOCK    0x2000   /* The open function will return without     */
                                /* waiting for the device to be ready or     */
                                /* available                                 */

#define  O_TEXT        0x4000   /* ASCII mode, <cr><lf> translates, CNTL-Z   */
#define  O_BINARY      0x8000   /* Mode is binary (no translation)           */

#define  O_RAW         O_BINARY /* Backward compatibility                    */

#endif
