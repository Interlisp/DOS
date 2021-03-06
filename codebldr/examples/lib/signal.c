/* SIGNAL.C illustrates setting up signal interrupt routines. Functions
 * illustrated include signal and raise.
 *
 * Since C I/O functions are not safe inside signal routines, the code
 * uses conditionals to use system-level DOS and OS/2 services. Another
 * option is to set global flags and do any I/O operations outside the
 * signal handler. To compile the OS/2 version, define the symbol OS2.
 */

#include <stdio.h>
#include <conio.h>
#include <signal.h>
#include <process.h>
#include <stdlib.h>
#if defined( OS2 ) && defined( _INTELC32_ )
   #define INCL_NOCOMMON
   #define INCL_NOPM
   #define INCL_VIO
   #define INCL_KBD
   #include <os2.h>
   #include <string.h>
#else
   #include <dos.h>
   #include <bios.h>
#endif

#ifdef _INTELC32_
   #include <stk.h>
   extern int    _controlC_option;
   #define CONTROL_C_ABORT() {_controlC_option = _STK_TERM; return;}
#else
   #define CONTROL_C_ABORT() abort();
#endif /* _INTELC32_ */
void ctrlchandler( void );          /* Prototypes */
void safeout( char *str );
int  safein( void );

void main()
{
#if defined( OS2 ) && defined( _INTELC32_ )
   printf ( "OS/2 functions not available on DOS\n" );
#else
   int ch;

   /* Modify CTRL+C behavior. */
   if( signal( SIGINT, ctrlchandler ) == SIG_ERR )
   {
      fprintf( stderr, "Couldn't set SIGINT\n" );
      abort();
   }

   /* Input loop illustrates results. */
   do
   {
      ch = getch();
      if( ch == 0 )
      {
         ch = getch();
         if( ch == 46 )      /* Treat ALT+C like CTRL+C */
            raise( SIGINT );
         else
            printf( "Extended code: %X\n", ch );
      }
      else
         printf( "ASCII code: %X\n", ch );
   } while( ch != 27 );        /* ESC code */
#endif
}

/* Handles SIGINT (CTRL+C) interrupt. */
void ctrlchandler()
{
   int c;
   char str[] = " ";

   /* Disallow CTRL+C during handler. */
   signal( SIGINT, SIG_IGN );

   safeout( "User break - abort processing? " );
   c = safein();
   str[0] = c;
   safeout( str );
   safeout( "\r\n" );
   if( (c == 'y') || (c == 'Y') )
      CONTROL_C_ABORT()
   else

      /* The CTRL+C interrupt must be reset to our handler since
       * by default it is reset to the system handler.
       */
      signal( SIGINT, ctrlchandler );
}

/* Outputs a string using system level calls. */
void safeout( char *str )
{
#if defined( OS2 ) && !defined( _INTELC32_ )
   VioWrtTTY( str, strlen( str ), 0 );
#else
   union REGS inregs, outregs;

   inregs.h.ah = 0x0e;
   while( *str )
   {
      inregs.h.al = *str++;
      int86( 0x10, &inregs, &outregs );
   }
#endif
}

/* Inputs a character using system level calls. */
int safein()
{
#if defined( OS2 ) && !defined( _INTELC32_ )
   KBDKEYINFO kki;

   KbdCharIn( &kki, IO_WAIT, 0 );
   return kki.chChar;
#else
   return _bios_keybrd( _KEYBRD_READ ) & 0xff;
#endif
}
