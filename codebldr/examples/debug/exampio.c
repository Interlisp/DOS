/*-------------------------------------------------------------------------
  EXAMPIO.C - This module contains a set of I/O interface functions to
  be used by a surrounding program.  A simple interface for console screen
  I/O is provided.
--------------------------------------------------------------------------*/
/*----------------------------HISTORY--------------------------------------
  09/27/90 - K.Reese & D. Duehren: Made generic.
  09/01/89 - Adam Trent: Adapted for use by the DB386 debugger.
  01/10/89 - Adam Trent: Created for use in the EXAMPLE program that is
  used by new users learning how to debug programs with the DB86 debugger.
--------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
  Bring in the main example program definitions.
--------------------------------------------------------------------------*/
#include "example.h"

extern char __getch();
extern void __putch(char);
extern void exit(int);
void crlf();

/*------------------------------------------------------------------------
  Here is a small set of C-lib type functions.
-------------------------------------------------------------------------*/

/* given a char, return it as upper case */
char toupper(c)
char c;
{
  c &= 0xff;
  if (c >= 'a' && c <= 'z')
     c &= ~0x20;
  return(c);
}

/* convert integer number to decimal number in ascii string */
void itoa(num, str)
int num;
char *str;
{
  int i;
  int d, w, j;
  char k;
  static char digits[] = "0123456789";

  w = num >= 0 ? num : (-num);
  str[0] = 0;
  i = 0;

  do                                  /* make string of ascii digits */
  {
    i++;
    d = w % 10;
    str[i] = digits[d];
    w = w / 10;
  }
  while (w != 0);

  if (num < 0)
    str[++i] = '-';                   /* if negative show sign */

  for (j = 0 ; j < i ; j++ )          /* reverse order of chars now */
  {
    k = str[i];
    str[i--] = str[j];
    str[j] = k;
  }
}

void dexit()
{
  crlf();
  exit(0);
}

/*=========================================================================
  output a character to stdout
==========================================================================*/
void cout(c)
char c;
{
  __putch(c);
}

/*=========================================================================
  beep the speaker on the computer.
==========================================================================*/
void beep()
{
  cout(BELL_KEY);
}

/*=========================================================================
  output a crlf pair to video
  Eg call:  crlf();
==========================================================================*/
void crlf()
{
   cout(CR_KEY);
   cout(LF_KEY);
}

/*=========================================================================
  clear screen.
==========================================================================*/
void cls()
{
  int i;

  for (i=0; i<SCR_ROWS; i++)
    crlf();
}

/*=========================================================================
  output a string to video
  Eg call:  stout("mystringlit"); stout(mystringvar);
==========================================================================*/
void stout(sp)
char *sp;
{
  char c;
  if (sp != NULLP)
    while((c = *sp++) != 0)
       cout(c);
}

/*========================================================================
  This is a function that shows text lines from an array of such.
========================================================================*/
void showtext(textarray)
TEXTPTR textarray;
{
  int i;
  cls();
  for (i=0; textarray[i] != NULLP; i++)
  {
    stout(textarray[i]);
    crlf();
  }
}

/*-------------------------------------------------------------------------
  convert lower nibble of byte to hex ascii
  Eg call:  cout(hexchar(0xa));
--------------------------------------------------------------------------*/
unsigned char hexchar(nib)
unsigned char nib;
{
  nib &= 0x0f;

  if (nib <= 0x09)
    nib |= 0x30;
  else
    nib += 55;

  return(nib);
}

/*=========================================================================
  output a hex byte to console
  Eg call:  hbout(mybytevar);
==========================================================================*/
void hbout(c)
char c;
{
  char numst[12];

  numst[0] = hexchar(c >> 4);
  numst[1] = hexchar(c);
  numst[2] = 0;
  stout(numst);
  stout(" ");
}

/*-------------------------------------------------------------------------
  output a hex word number to console
  Eg call:  hwout(mywordvar);
--------------------------------------------------------------------------*/
void hwout(w)
unsigned int w;
{
  char numst[12];

  numst[0] = hexchar(w >> 12);
  numst[1] = hexchar(w >> 8 );
  numst[2] = hexchar(w >> 4 );
  numst[3] = hexchar(w);
  numst[4] = 0;
  stout(numst);
}

/*=========================================================================
  output a short pointer as a dw hex value.
  Eg call:  hfpout(mypointervar);
==========================================================================*/
void haddrout(cfp)
char *cfp;
{
  char numst[16];

  numst[0] = hexchar((long)cfp >> 28);
  numst[1] = hexchar((long)cfp >> 24);
  numst[2] = hexchar((long)cfp >> 20);
  numst[3] = hexchar((long)cfp >> 16);
  numst[4] = hexchar((long)cfp >> 14);
  numst[5] = hexchar((long)cfp >>  8);
  numst[6] = hexchar((long)cfp >>  4);
  numst[7] = hexchar((char)(long)cfp);
  numst[8] = 0;
  stout(numst);
}

/*=========================================================================
  cin - Get user keyin character, consume LF.
==========================================================================*/
char cin()
{
  char key;

  key = ls_byte((char)__getch());

  return(key);
}

/*-------------------------------------------------------------------------
  inhnib - input a hex character from user; beep if illegal char typed.
    Return binary of hex nibble of key pressed.
--------------------------------------------------------------------------*/
unsigned int inhnib()
{
  unsigned int key;
  int done = FALSE;

  while (!done)
  {
    key = toupper(cin());

    if ((key > 0x002f && key < 0x003a))
    {
      cout(key);
      key -= 0x30;
      key &= 0x000f;
      done = TRUE;
    }
    else
      if ((key > 0x0040 && key < 0x0047))
      {
        cout(key);
        key -= 0x37;
        key &= 0x000f;
        done = TRUE;
      }
      else
        beep();
  }

  return (key);
}

/*=========================================================================
  inptr - input a memory pointer from user and return it to caller.
==========================================================================*/
char *inptr()
{
  ULONG dw;
  unsigned int nib;

  nib = inhnib();
  dw  = (ULONG)nib << 28;
  nib = inhnib();
  dw |= (ULONG)nib << 24;
  nib = inhnib();
  dw |= (ULONG)nib << 20;
  nib = inhnib();
  dw |= (ULONG)nib << 16;
  nib = inhnib();
  dw |= (ULONG)nib << 12;
  nib = inhnib();
  dw |= (ULONG)nib << 8;
  nib = inhnib();
  dw |= (ULONG)nib << 4;
  nib = inhnib();
  dw |= (ULONG)nib;

  return ((char *)dw);
}

/*=========================================================================
  Here is a function to pause with a "Press Any Key" prompt.
==========================================================================*/
void pause()
{
  crlf();
  stout("               ==== Press Any Key to Continue ====");
  cin();
}
