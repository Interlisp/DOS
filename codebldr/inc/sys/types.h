/* types.h - various return types for system level functions
 * $Version: 1.5 $
 * Copyright 1989,90,91 Intel Corporation, ALL RIGHTS RESERVED.
 */

#ifndef _typesh
#define _typesh

#ifndef _dev_t
typedef unsigned dev_t;
#define _DEV_T_DEFINED
#define _dev_t
#endif

#ifndef _fpos_t
typedef unsigned fpos_t;
#define _FPOS_T_DEFINED
#define _fpos_t
#endif

#ifndef _gid_t
typedef unsigned gid_t;
#define _GID_T_DEFINED
#define _gid_t
#endif

#ifndef _ino_t
typedef unsigned ino_t;
#define _INO_T_DEFINED
#define _ino_t
#endif

#ifndef _mode_t
typedef unsigned mode_t;
#define _MODE_T_DEFINED
#define _mode_t
#endif

#ifndef _nlink_t
typedef unsigned nlink_t;
#define _NLINK_T_DEFINED
#define _nlink_t
#endif

#ifndef _off_t
typedef int off_t;
#define _OFF_T_DEFINED
#define _off_t
#endif

#ifndef _pid_t
typedef int pid_t;
#define _PID_T_DEFINED
#define _pid_t
#endif

#ifndef _size_t
typedef unsigned size_t;
#define _SIZE_T_DEFINED
#define _size_t
#endif

#ifndef _time_t
typedef long time_t;
#define _TIME_T_DEFINED
#define _time_t
#endif

#ifndef _uid_t
typedef unsigned uid_t;
#define _UID_T_DEFINED
#define _uid_t
#endif

#endif /* _typesh */
