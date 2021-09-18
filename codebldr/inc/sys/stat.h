/* stat.h - file stat structure
 * $Version: 1.9 $
 * Copyright 1988,89,90,91 Intel Corporation, ALL RIGHTS RESERVED.
 */

#ifndef _stath
#define _stath

#ifndef _typesh
#include <sys/types.h>
#endif

/*
 * File types:
 */
#define S_IFMT   0xF000         /* Mask                                      */
#define S_IFIFO  0x1000         /* FIFO file                                 */
#define S_IFCHR  0x2000         /* Character device                          */
#define S_IFDIR  0x4000         /* Directory                                 */
#define S_IFBLK  0x6000         /* Block device                              */
#define S_IFREG  0x8000         /* Regular file                              */

/*
 * Macros to test file types:
 */
#define S_ISDIR(m)  (((m) & S_IFMT ) == S_IFDIR)
#define S_ISCHR(m)  (((m) & S_IFMT ) == S_IFCHR)
#define S_ISBLK(m)  (((m) & S_IFMT ) == S_IFBLK)
#define S_ISREG(m)  (((m) & S_IFMT ) == S_IFREG)
#define S_ISFIFO(m) (((m) & S_IFMT ) == S_IFIFO)

/*
 * File access permission levels (in octal):
 */
#define S_ISGID    0002000      /* Set group ID on execution                 */
#define S_ISUID    0001000      /* Set user ID on execution                  */

#define S_IREAD    0000400      /* Pre-POSIX definition                      */
#define S_IWRITE   0000200      /* Pre-POSIX definition                      */
#define S_IEXEC    0000100      /* Pre-POSIX definition                      */

#define S_IARCHIVE 0100000      /* DOS-specific mode                         */
#define S_ISUBDIR  0040000      /* DOS-specific mode                         */
#define S_IVOLUME  0020000      /* DOS-specific mode                         */
#define S_ISYSTEM  0010000      /* DOS-specific mode                         */
#define S_IHIDDEN  0004000      /* DOS-specific mode                         */


/*
 * File owner class (in octal):
 */
#define S_IRWXU    0000700      /* Mask for file owner class                 */
#define S_IRUSR    0000400      /* Read permission for file owner class      */
#define S_IWUSR    0000200      /* Write permission for file owner class     */
#define S_IXUSR    0000100      /* Execute/search permission for file owner  */

/*
 * File group class (in octal):
 */
#define S_IRWXG    0000070      /* Mask for file group class                 */
#define S_IRGRP    0000040      /* Read permission for file group class      */
#define S_IWGRP    0000020      /* Write permission for file group class     */
#define S_IXGRP    0000010      /* Execute/search permission for file group  */

/*
 * File other class (in octal):
 */
#define S_IRWXO    0000007      /* Mask for file other class                 */
#define S_IROTH    0000004      /* Read permission for file other class      */
#define S_IWOTH    0000002      /* Write permission for file other class     */
#define S_IXOTH    0000001      /* Execute/search permission for file other  */


/*
 * Structure used by _dos_findfirst
 */
#ifndef _find_t
#pragma noalign (find_t)
struct find_t {
        char reserved[21];      /* Reserved for DOS                          */
        char attrib;            /* Returned attribute                        */
        unsigned short wr_time; /* Time file last written                    */
        unsigned short wr_date; /* Date file last written                    */
        long  size;             /* Size of file                              */
        char name[13];          /* String containing the filename            */
        };
#define _FIND_T_DEFINED
#define _find_t
#endif

/*
 * File stat structure:
 */
#pragma align (stat)
struct stat {
    dev_t   st_dev;             /* ID of device containing this file         */
    ino_t   st_ino;             /* File serial number                        */
    mode_t  st_mode;            /* File mode                                 */
    nlink_t st_nlink;           /* Number of links                           */
    uid_t   st_uid;             /* User ID of the file's owner               */
    gid_t   st_gid;             /* Group ID of the file's group              */
    dev_t   st_rdev;            /* Same as  st_dev                           */
    off_t   st_size;            /* File size in bytes (regular files only)   */
    time_t  st_atime;           /* Time of last access                       */
    time_t  st_mtime;           /* Time of last data modification            */
    time_t  st_ctime;           /* Time of last file status change           */
};
#define _STAT_DEFINED

/*
 * Function prototypes:
 */
int fstat(int, struct stat *);
int stat(const char *, struct stat *);

#endif /* _stath */

