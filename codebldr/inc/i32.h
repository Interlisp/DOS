/* i32.h - i386(TM) processor specific header file
 * $Version: 1.21 $
 * Copyright 1990-1992 Intel Corporation.  All Rights Reserved.
 */

#if !defined(_I32_)
#define _I32_

#pragma _builtin_("_getflags"==8)
unsigned short  _getflags(void);

#pragma _builtin_("_setflags"==9)
void    _setflags(unsigned short);

#pragma _builtin_("_inword"==11)
unsigned int    _inword(unsigned short);

#pragma _builtin_("_outword"==13)
void    _outword(unsigned short, unsigned int);

#pragma _builtin_("_initrealmathunit"==14)
void    _initrealmathunit(void);

#pragma _builtin_("_setrealmode"==15)
void    _setrealmode(unsigned short);

#pragma _builtin_("_getrealerror"==17)
unsigned short  _getrealerror(void);

#pragma align("i387_protected_addr")
struct i387_protected_addr
{
		unsigned	ip_offset	: 32;
		unsigned	cs_sel		: 16;
		unsigned	opcode		: 11, : 5;
		unsigned	op_offset	: 32;
		unsigned	op_sel		: 16, : 16;
};

#pragma align("i387_real_address")
struct i387_real_address
{
		unsigned	ip1		: 16, : 16;
		unsigned	opcode	: 11, : 1;
		unsigned	ip2		: 16, : 4;
		unsigned	op1		: 16, : 16, : 12;
		unsigned	op2		: 16, : 4;
};

union i387_address
{
        struct i387_real_address real;
        struct i387_protected_addr prot;
};

#pragma align("i387_environment")
struct i387_environment
{
        unsigned                control: 16, : 16;
        unsigned                status : 16, : 16;
        unsigned                tag    : 16, : 16;
        union i387_address      ptrs_n_opcode;
};

#pragma noalign("i87_tempreal")
struct i87_tempreal
{
        char            significand[8];
        unsigned        exponent: 15;
#if defined(SBITFIELD)
        signed          sign: 1;
#else
        unsigned        sign: 1;
#endif
};

typedef struct i87_tempreal     tempreal_t;

struct i387_state
{
        struct i387_environment environment;
        tempreal_t              stack[8];
};

#pragma _builtin_("_saverealstatus"==18)
void    _saverealstatus(struct i387_state *savearea);

#pragma _builtin_("_restorerealstatus"==19)
void    _restorerealstatus(struct i387_state const *savearea);

#define I87_INITIAL_CW_VALUE		0x037f

#define I87_INVALID_OPERATION       0x0001
#define I87_DENORMALIZED_OPERAND    0x0002
#define I87_ZERO_DIVIDE             0x0004
#define I87_OVERFLOW                0x0008
#define I87_UNDERFLOW               0x0010
#define I87_PRECISION               0x0020

#define I87_CONTROL_PRECISION       0x0300
#define I87_PRECISION_24_BIT        0x0000
#define I87_PRECISION_53_BIT        0x0200
#define I87_PRECISION_64_BIT        0x0300

#define I87_CONTROL_ROUNDING        0x0C00
#define I87_ROUND_NEAREST           0x0000
#define I87_ROUND_DOWN              0x0400
#define I87_ROUND_UP                0x0800
#define I87_ROUND_CHOP              0x0C00

#pragma _builtin_("_blockinbyte"==20)
void    _blockinbyte(unsigned short, unsigned char *, unsigned int);

#pragma _builtin_("_blockinword"==21)
void    _blockinword(unsigned short, unsigned int *, unsigned int);

#pragma _builtin_("_blockoutbyte"==22)
void    _blockoutbyte(unsigned short, unsigned char const *, unsigned int);

#pragma _builtin_("_blockoutword"==23)
void    _blockoutword(unsigned short, unsigned int const *, unsigned int);

#ifndef _conioh_builtin
#define _conioh_builtin

#pragma _builtin_("_inbyte"==10)
unsigned char   _inbyte(unsigned short);

#pragma _builtin_("_outbyte"==12)
void    _outbyte(unsigned short, unsigned char);

#pragma _builtin_("_inhword"==67)
unsigned short  _inhword(unsigned short);

#pragma _builtin_("_outhword"==68)
void    _outhword(unsigned short, unsigned short);

#endif /* _conioh_builtin */

#pragma _builtin_("_blockinhword"==69)
void    _blockinhword(unsigned short, unsigned short *, unsigned int);

#pragma _builtin_("_blockouthword"==70)
void    _blockouthword(unsigned short, unsigned short const *, unsigned int);

#pragma _builtin_("_byte_rol"==74)
unsigned char   _byte_rol(unsigned char, unsigned int);

#pragma _builtin_("_byte_ror"==75)
unsigned char   _byte_ror(unsigned char, unsigned int);

#ifndef _stdlibh_builtin
#define _stdlibh_builtin

#pragma _builtin_("_hword_rol"==76)
unsigned short  _hword_rol(unsigned short, unsigned int);

#pragma _builtin_("_hword_ror"==77)
unsigned short  _hword_ror(unsigned short, unsigned int);

#pragma _builtin_("_word_rol"==78)
unsigned int    _word_rol(unsigned int, unsigned int);

#pragma _builtin_("_word_ror"==79)
unsigned int    _word_ror(unsigned int, unsigned int);

#endif /* _stdlibh_builtin */

#if _ARCHITECTURE_ > 386
#pragma _builtin_("_byteswap"==47)
unsigned int	_byteswap(unsigned int);
#endif

#pragma _builtin_("_resume"==73)
void    _resume(void);

#ifndef _dosh_builtin
#define _dosh_builtin

#define _FLAG_CARRY     0x0001
#define _FLAG_PARITY    0x0004
#define _FLAG_AUXCARRY  0x0010
#define _FLAG_ZERO      0x0040
#define _FLAG_SIGN      0x0080
#define _FLAG_TRAP      0x0100
#define _FLAG_INTERRUPT 0x0200
#define _FLAG_DIRECTION 0x0400
#define _FLAG_OVERFLOW  0x0800
#define _FLAG_IOPL      0x3000
#define _FLAG_NESTED    0x4000
#define _FLAG_RESUME    0x10000
#define _FLAG_VM        0x20000

#pragma _builtin_("_get_stk_frame"==81)
void    *_get_stk_frame(void);

#pragma _builtin_("_chain_intr"==82)
void    _chain_intr(void(* _intr_fnc)(void));

#endif /* _dosh_builtin */

#endif
