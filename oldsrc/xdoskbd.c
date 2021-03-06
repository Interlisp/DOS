#include <i32.h>                       /* "#pragma interrupt" & '_chain_intr'*/
#include <dos.h>                       /* defines REGS & other structs       */
#include <stdio.h>                     /* define NULL                        */
#include <conio.h>
#include <time.h>
#include <stk.h>


#define FALSE        0
#define TRUE         1

#define KBD_COMMAND_PORT	0x64
#define KBD_ENABLE		0xAE
#define KBD_DISENABLE		0xAD

#define PORT_A			0x60
#define PORT_8042		0x60
#define KBD_SCAN_CODE_PORT	0x60

#define KBD_resend		0xFE
#define KBD_ack			0xFA
#define KBD_echo_responce	0xEE
#define KBD_failiure		0xFD
#define KBD_prefix		0xE0
#define KBD_overflow		0x00

#define INTA00			0x20 /* The 8259 port */
#define ENDOFINTERRUPT		0x20

void      (*prev_int_09)();	/* keeps address of previous 09 handlr*/

#pragma interrupt(DOSkbd)
void  DOSkbd(void);		/* the control-c (int 0x23) handler   */
char kbd_inb[20];
unsigned char inchar;
unsigned char char1;

unsigned int expecting_kbd_ack = 0;

unsigned char DOSLispKeyMap_101[0xFF] =
{
/*         0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f */

/* 0*/  0xff,  33,  32,  17,  16,   1,   0,   2,   4,  53,  22,   8,  10,  59,  15,  34,
/* 1*/    19,  18,   3,  48,  49,  51,   6,  23,  25,  11,  58,  29,  44,  36,  21,  20,
/* 2*/     5,  35,  50,  52,  38,   9,  26,  43,  28,  45,  41, 105,  40,  24,  37,   7,
/* 3*/    39,  54,  55,  27,  42,  12,  60,  95,  31,  57,  56,  97,  99, 100,  67,  68,
/* 4*/   101,  66, 104,  80, 106,  73,  74,  81,  82,  83,  96,  84,  85,  87, 102,  94,
/* 5*/    69,  70,  98,  13,0xff,0xff,0xff, 107, 108,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
/* 6*/  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
/* 7*/  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
};

unsigned char stat, stat1;

main()
{
  init_keyboard();
  DOS_led( 1, 0, 1);
  while (TRUE)
    {
      if ( inchar > 0 ) {
	printf("%x --\n", inchar);
      }
      if (inchar == 0x4F) {
	return;
      }
      inchar = 0;
    }
  DOS_led( 0, 1, 0);
  DOSkbd_before_exit();
}

void DOSkbd()
{
  
  _XSTACK *ebp;			/* Real-mode handler stack frame */

  ebp = (_XSTACK *) _get_stk_frame(); /* Get stack frame address */
  ebp->opts |= _STK_NOINT;	/* Bypass real-mode handler */

  disable_DOSkeyboard();
  inchar = inp( KBD_SCAN_CODE_PORT );
  switch ( inchar ) {
  case KBD_overflow :
    break;
  case KBD_prefix :
    outp (INTA00, ENDOFINTERRUPT);
    break;
  case KBD_echo_responce :
    break;
  case KBD_ack :		/* It's an ack, flag that it has been found */
    expecting_kbd_ack = 0;
    inchar = 0;
    outp (INTA00, ENDOFINTERRUPT);
    break;
  case KBD_failiure :
    break;
  case KBD_resend :
    break;
  default:
    break;
  }
  enable_DOSkeyboard();

}


enable_DOSkeyboard()
{
  outp( KBD_COMMAND_PORT, KBD_ENABLE);
}

disable_DOSkeyboard()
{
  outp( KBD_COMMAND_PORT, KBD_DISENABLE);
}

keyboardtype(dummy)
     int dummy;
{
}

init_keyboard()
{
  prev_int_09 = _dos_getvect(0x09); /* get addr of currnt 09 hndlr, if any*/
  _dos_setvect(0x09, DOSkbd);	/* hook our int handler to interrupt  */
}

DOSkbd_before_exit()
{
  /* Restore the kbd */
  _dos_setvect(0x09, prev_int_09); /* unhook our handlr, install previous*/
}

unsigned char bell_status_word;

DOS_beep(onoff, freq)
{
  if(onoff ==1 ){
    bell_status_word = inp( 0x61 );
    outp( 0x61, bell_status_word | 0x3 ); /* Turn on the speaker */
    /* Prepare timer by sending 10111100 to port 43. */
    outp( 0x43, 0xb6 );

    /* Divide input frequency by timer ticks per second and
     * write (byte by byte) to timer. */
    outp( 0x42, (char)(1193180L / freq) );
    outp( 0x42, (char)((1193180L / freq) >> 8) );
  } else {
    outp( 0x61, bell_status_word & ~0x3 ); /* Turn off the speaker (with */
    /* bits 0 and 1). */

  }
}

DOS_led (cl, nl, sl)
{
  
  expecting_kbd_ack = 1;
  outp(PORT_A, 0xED);
  while(expecting_kbd_ack){};
  expecting_kbd_ack = 1;
  outp(PORT_A, (((cl == 1) << 2) | ((nl == 1) << 1) | (sl == 1)));
  while(expecting_kbd_ack){};
}
