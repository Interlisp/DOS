/* errno.h - error number definitions
 * $Version: 1.10 $
 * Copyright 1988,89,90,91 Intel Corporation, ALL RIGHTS RESERVED.
 */

#ifndef _errnoh
#define _errnoh

#ifdef _MT
#ifndef _reenth
#include <reent.h>
#endif
#endif /* _MT */

#ifndef errno
#ifdef _MT
#define errno        ((_thread_ptr()->errno))    /* Library error variable */
#else
extern	int			 errno;
#endif /* _MT */
#endif 

#define EZERO           0           /* No error                              */
#define EPERM           1           /* Operation not permitted               */
#define ENOENT          2           /* No such file or directory             */
#define ESRCH           3           /* No such process                       */
#define EINTR           4           /* Interrupted function call             */
#define EIO             5           /* I/O error                             */
#define ENXIO           6           /* No such device or address             */
#define E2BIG           7           /* Argument list too long                */
#define ENOEXEC         8           /* File not executable                   */
#define EBADF           9           /* Invalid file descriptor               */
#define ECHILD         10           /* No child process                      */
#define EAGAIN         11           /* Resource temporarily unavailable      */
#define ENOMEM         12           /* Not enough space                      */
#define EACCES         13           /* File access denied                    */
#define EFAULT         14           /* Bad address                           */
#define ENOTBLK        15           /* Block device required                 */
#define EBUSY          16           /* System resource busy                  */
#define EEXIST         17           /* File already exists                   */
#define EXDEV          18           /* Improper link                         */
#define ENODEV         19           /* No such device                        */
#define ENOTDIR        20           /* Not a directory                       */
#define EISDIR         21           /* Is a directory                        */
#define EINVAL         22           /* Invalid argument or operation         */
#define ENFILE         23           /* System file table overflow            */
#define EMFILE         24           /* Too many open files for process       */
#define ENOTTY         25           /* Inappropriate I/O control             */
#define ETXTBSY        26           /* Text file busy                        */
#define EFBIG          27           /* File too large                        */
#define ENOSPC         28           /* No space left on device               */
#define ESPIPE         29           /* Illegal seek                          */
#define EROFS          30           /* Read only file system                 */
#define EMLINK         31           /* Too many links                        */
#define EPIPE          32           /* Broken pipe                           */
#define EDOM           33           /* Math arg out of domain of func        */
#define ERANGE         34           /* Math functon result not representable */
#define EUCLEAN        35           /* Reserved                              */
#define EDEADLOCK      36           /* Locking violation                     */
#define EDEADLK        37           /* Resource deadlock avoided             */
#define EFREE          38           /* Bad free pointer                      */
#define ENAMETOOLONG   39           /* Filename too long                     */
#define ENOLCK         40           /* No locks available                    */
#define ENOSYS         41           /* Function not implemented              */
#define ENOTEMPTY      42           /* Directory not empty                   */
#define ESIGNAL        43           /* Bad signal vector                     */

#define _NUM_ERR_NUMS  44           /* For strerror()                        */

#endif /* _errnoh */

