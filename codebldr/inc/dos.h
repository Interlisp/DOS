/* dos.h - defines data structures for DOS access
 * $Version: 1.32 $
 * Copyright 1988,89,90,90 Intel Corporation, ALL RIGHTS RESERVED.
 */

#ifndef _dosh
#define _dosh

extern const unsigned   _dataseg;       /* Value of DS register              */
extern unsigned short   _osversion;     /* _osmajor maps onto the low byte   */
                                        /*   and _osminor onto the high byte */
/*
 * _hardresume status values:
 */
#define _HARDERR_IGNORE  0x00           /* Ignore the error, i.e., continue  */
#define _HARDERR_RETRY   0x01           /* Retry the failed operation        */
#define _HARDERR_ABORT   0x02           /* Issue INT 23h to abort program    */
#define _HARDERR_FAIL    0x03           /* Fail the operation                */

/*
 * File attribute masks:
 */
#define _A_NORMAL        0x00           /* Normal file access, read/write OK */
#define _A_RDONLY        0x01           /* File is read-only                 */
#define _A_HIDDEN        0x02           /* File is hidden                    */
#define _A_SYSTEM        0x04           /* File is a system file             */
#define _A_VOLID         0x08           /* File holds volume ID              */
#define _A_SUBDIR        0x10           /* File is a subdirectory            */
#define _A_ARCH          0x20           /* File needs to be archived         */


#define DATASEG()      _dataseg
#define FP_OFF(_p)    ((unsigned)_p)
#define FP_SEG(_p)     _dataseg
#define _NP_OFF(_p)   ((unsigned)_p)
#define _NP_SEG(_p)    _dataseg

#if !_FAR_DATA_
#define P_SEG(_p) _NP_SEG(_p)
#define P_OFF(_p) _NP_OFF(_p)
#else
#define P_SEG(_p) FP_SEG(_p)
#define P_OFF(_p) FP_OFF(_p)
#endif

#ifndef _FAR
#define _FAR
#define _HUGE
#define _NEAR
#endif

#ifndef _doserror
#pragma align (DOSERROR)
struct DOSERROR {
    int exterror;
    char class, action, locus;
};
#define _doserror
#define _DOSERROR_DEFINED
#endif

/*
 * Structure used by loadexec function:
 */
#pragma align (pblock)
struct pblock {
   unsigned env;                        /* Segment address of environment    */
   char _FAR *com_line;                 /* Program command line              */
   char _FAR *fcb1;
   char _FAR *fcb2;
};

/*
 * Used by sysint() and sysint21():
 */
#pragma align (regval)
struct regval {
    unsigned int  ax, bx, cx, dx, si, di, ds, es, bp;
};

#ifndef _regs
#pragma align (DWORDREGS)
#pragma align (WORDREGS)
#pragma align (BYTEREGS)
union REGS {
  struct DWORDREGS { unsigned eax, ebx, ecx, edx, esi,
                              edi, cflag, eflags, ebp;} w;

#if 1  /* 32-bit implementation */
  struct WORDREGS { unsigned  ax, bx, cx, dx, si,
                              di, cflag, flags, bp;} x;
#else  /* 16-bit implementation */
  struct WORDREGS { unsigned  ax:16, :16, bx:16, :16, cx:16, :16,
                              dx:16, :16, si:16, :16, di:16, :16,
                              cflag:16, :16, flags:16, :16, bp:16, :16;} x;
#endif

  struct BYTEREGS { unsigned  al:8, ah:8, :16, bl:8, bh:8, :16,
                              cl:8, ch:8, :16, dl:8, dh:8, :16;} h;
};

#pragma align (SREGS)
struct SREGS {
    unsigned short es, cs, ss, ds, fs, gs;
};

#define _regs
#define _REGS_DEFINED
#endif  /* _regs */

/*
 * Structure used by _dos_findfirst:
 */
#ifndef _find_t
#pragma noalign (find_t)
struct find_t {
    char reserved[21];                  /* Reserved for DOS                  */
    char attrib;                        /* Returned attribute                */
    unsigned short wr_time;             /* Time file last written            */
    unsigned short wr_date;             /* Date file last written            */
    long  size;                         /* Size of file                      */
    char name[13];                      /* String containing the filename    */
    };
#define _find_t
#define _FIND_T_DEFINED
#endif

/*
 * Structure used by _dos_getdate and _dos_setdate:
 */
#ifndef _datetime_t
#pragma align (dosdate_t)
#pragma align (dostime_t)
struct dosdate_t {
    unsigned char day;                    /* Range: 1-31                     */
    unsigned char month;                  /* Range: 1-12                     */
    unsigned short year;                  /* Range: 1980-2099                */
    unsigned char dayofweek;              /* Range: 0-6 (0=Sun, 1=Mon, etc.) */
    };

/*
 * Structure used by _dos_gettime and _dos_settime:
 */
struct dostime_t {
    unsigned char hour;                                       /* Range: 0-23 */
    unsigned char minute;                                     /* Range: 0-59 */
    unsigned char second;                                     /* Range: 0-59 */
    unsigned char hsecond;                                    /* Range: 0-99 */
    };
#define _datetime_t
#define _DATETIME_T_DEFINED
#endif

/*
 * Structure used by _dos_getdiskfree:
 */
#ifndef _diskfree_t
#pragma align (diskfree_t)
struct diskfree_t {
    unsigned short total_clusters;
    unsigned short avail_clusters;
    unsigned short sectors_per_cluster;
    unsigned short bytes_per_sector;
    };
#define _diskfree_t
#define _DISKFREE_T_DEFINED
#endif

#pragma interrupt ( _harderr_handler )

/*
 * Function prototypes:
 */
int         bdos(int, unsigned int, unsigned int);
void _FAR  _ctlcint(void);
void _FAR  _div0int(void);
unsigned   _dos_allocmem(unsigned, unsigned *);
unsigned   _dos_close(int);
unsigned   _dos_creat(const char *, unsigned, int *);
unsigned   _dos_creatnew(const char *, unsigned, int *);
unsigned   _dos_findfirst(const char *, unsigned, struct find_t *);
unsigned   _dos_findnext(struct find_t *);
unsigned   _dos_freemem(unsigned);
void       _dos_getdate(struct dosdate_t *);
unsigned   _dos_getdiskfree(unsigned, struct diskfree_t *);
void       _dos_getdrive(unsigned *);
void     (*_dos_getexcept(unsigned))();
unsigned   _dos_getfileattr(const char *, unsigned *);
unsigned   _dos_getftime(int, unsigned *, unsigned *);
void       _dos_gettime(struct dostime_t *);
void     (*_dos_getvect(unsigned))();
unsigned   _dos_open(const char *, unsigned, int *);
unsigned   _dos_read(int, void *, unsigned, unsigned *);
unsigned   _dos_setblock(unsigned, unsigned, unsigned *);
unsigned   _dos_setdate(struct dosdate_t *);
void       _dos_setdrive(unsigned, unsigned *);
void       _dos_setexcept(unsigned, void (*)());
unsigned   _dos_setfileattr(const char *, unsigned);
unsigned   _dos_setftime(int, unsigned, unsigned);
unsigned   _dos_settime(struct dostime_t *);
void       _dos_setvect(unsigned, void (*)());
unsigned   _dos_write(int, const void *, unsigned, unsigned *);
int         dosexterr(struct DOSERROR *);
void _FAR  _fpeint(void);
void       _harderr(void (*)());
void       _hardresume(int);
void       _hardretn(int);
int         intdos(const union REGS *, union REGS *);
int         intdosx(const union REGS *, union REGS *, struct SREGS *);
int         loadexec(char _FAR *, struct pblock _FAR *, int);
long        ptrdiff(void _FAR *, void _FAR *);
void       _quit(int);
void        segread(struct SREGS *);
int         sysint(unsigned, const struct regval *, struct regval *);
int         sysint21(const struct regval *, struct regval *);

#ifndef _int86
int         int86(int, const union REGS *, union REGS *);
int         int86x(int, const union REGS *, union REGS *, struct SREGS *);
#define int386(_a,_b,_c)     int86(_a,_b,_c)
#define int386x(_a,_b,_c,_d) int86x(_a,_b,_c,_d)
#define _int86
#endif

#ifndef _dpmi
/* extern unsigned int _dpmierrno;   Error codes not suported by DPMI 0.9 */
unsigned  _dpmi_disable(void);
unsigned  _dpmi_enable(void);
int       _dpmi_lockregion ( void *, int );
int       _dpmi_unlockregion ( void *, int );
int       _dpmi_allocphysicalmapping( void *, unsigned int, void ** );
int       _dpmi_freephysicalmapping( void * );
#define _dpmi
#endif

/*
 * Prototypes of functions not supported by C Code Builder(TM) Libraries:
 */
void     _dos_keep(unsigned, unsigned);

#ifndef _dosh_builtin
#define _dosh_builtin
/*
 * Processor status flag values:
 */
#define _FLAG_CARRY      0x0001         /* Carry flag                        */
#define _FLAG_PARITY     0x0004         /* Parity flag                       */
#define _FLAG_AUXCARRY   0x0010         /* Auxillary carry flag              */
#define _FLAG_ZERO       0x0040         /* Zero flag                         */
#define _FLAG_SIGN       0x0080         /* Sign flag                         */
#define _FLAG_TRAP       0x0100         /* Trap flag                         */
#define _FLAG_INTERRUPT  0x0200         /* Interrupt enable flag             */
#define _FLAG_DIRECTION  0x0400         /* Direction flag                    */
#define _FLAG_OVERFLOW   0x0800         /* Overflow flag                     */
#define _FLAG_IOPL       0x3000         /* IO privilege level mask           */
#define _FLAG_NESTED     0x4000         /* Nested task flag                  */
#define _FLAG_RESUME    0x10000         /* Resume flag                       */
#define _FLAG_VM        0x20000         /* Virtual 86 mode                   */

/*
 * In-line implementation of functions:
 */
#pragma    _builtin_("_get_stk_frame"==81)
void      *_get_stk_frame(void);

#pragma    _builtin_("_chain_intr"==82)
void       _chain_intr(void(* _intr_fnc)(void));

#pragma _builtin_("_enable"==4)
unsigned short  _enable(void);

#pragma _builtin_("_disable"==5)
unsigned short  _disable(void);
#endif /* _dosh_builtin */

#endif /* _dosh */
