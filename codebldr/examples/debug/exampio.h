/*-------------------------DESCRIPTION-------------------------------------
  EXAMPIO.H - External Definitions for EXAMPIO functions.  EXAMPIO.H is
  meant to be #included by modules that make use of the EXAMPIO functions.
--------------------------------------------------------------------------*/
/*---------------------------HISTORY---------------------------------------
09/01/89 - A.Trent: Adapted for use by the DB386 debugger.
08/06/89 - A.Trent: Adapted for use by the DB960 debugger.
10/07/88 - Adam Trent: Created as EXAMPLE program for DB86 debugger.
--------------------------------------------------------------------------*/
extern void beep(void);
extern char cin(void);
extern void cls(void);
extern void cout(char);
extern void crlf(void);
extern void dexit(void);
extern void hbout(unsigned char);
extern void haddrout(char *);
extern char *inptr(void);
extern void itoa(int,char *);
extern void pause(void);
extern void showtxt(TEXTPTR);
extern void stout(char *);
extern char toupper(char);
