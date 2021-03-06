/* conio.h - low level I/O function prototypes
 * $Version: 1.12 $
 * Copyright 1988,89,90,91 Intel Corporation, ALL RIGHTS RESERVED
 */

#ifndef _conioh
#define _conioh

/*
 * Function prototypes:
 */
char    *cgets(char *);
int      cprintf(const char *, ...);
int      cputs(const char *);
int      cscanf(const char *, ...);
int      getch(void);
int      getche(void);
int      inp(unsigned);
unsigned inpw(unsigned);
int      kbhit(void);
int      outp(unsigned, int);
unsigned outpw(unsigned, unsigned);
int      putch(int);
int      ungetch(int);

/*
 * Compiler built-ins:
 */
#ifndef _conioh_builtin
#define _conioh_builtin

#pragma _builtin_("_inbyte"==10)
unsigned char   _inbyte(unsigned short);

#pragma _builtin_("_outbyte"==12)
void    _outbyte(unsigned short,
                 unsigned char);

#pragma _builtin_("_inhword"==67)
unsigned short  _inhword(unsigned short);

#pragma _builtin_("_outhword"==68)
void    _outhword(unsigned short,
                  unsigned short);

#endif /* _conioh_builtin */

/*
 * Macro implementation of stdio functions:
 */
#define inp(_f)       ( (int) _inbyte( (unsigned short)(_f) ) )
#define inpw(_f)      ( (unsigned) _inhword( (unsigned short)(_f) ) )
#define outp(_f,_g)   _outbyte( (unsigned short)(_f), (unsigned char)(_g) )
#define outpw(_f,_g)  _outhword( (unsigned short)(_f), (unsigned short)(_g) )

#endif /* _conioh */
