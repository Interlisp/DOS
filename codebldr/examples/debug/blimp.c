/*---------------------------------------------------------------------------
  BLIMP.C - This module contains a function, blimp_fly, that displays
  an image of a blimp advertising Intel debuggers.
----------------------------------------------------------------------------*/
/*-------------------------HISTORY-------------------------------------------
  09/27/90 - K.Reese & D. Duehren: Made generic.
  09/01/89 - A.Trent: Adapted for use by the DB386 debugger.
  08/06/89 - A.Trent & G.Regnier: Adapted for use by the DB960 debugger.
  07/10/89 - Adam Trent: Included as part of the EXAMPLE program to be used
    as an example program for debugging with the DB86 symbolic source level
    software debugger.
----------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------
  Bring in the main example program definitions.
--------------------------------------------------------------------------*/
#include "example.h"

/*-------------------------------------------------------------------------
  Bring in external definitions for the screen I/O functions.
--------------------------------------------------------------------------*/
#include "exampio.h"

/*---------------------------------------------------------------------------
  Here is the actual blimp!
----------------------------------------------------------------------------*/

char blimp0[80] = { "              * * * * * * * * * * * *        __                           " };
char blimp1[80] = { "           *     _________________    ***    ||                           " };
char blimp2[80] = { "         *      | Intel Debuggers |      **  ||        __________________ " };
char blimp3[80] = { "       (         -----------------         **||-------< The Bug Killers! >" };
char blimp4[80] = { "         *                               **  ||        ------------------ " };
char blimp5[80] = { "           *                          ***    ||                           " };
char blimp6[80] = { "              * * * * * * * * * * * *        --                           " };
char blimp7[80] = { "                      |   |-|                                             " };
char blimp8[80] = { "                      -----                                               " };
char blimp9[80] = { "                                                                          " };

char brag_message[] = {
                    "         Easy to Learn and Easy to Use Source-level Debugging " };


/*-------------------------------------------------------------------------
  Here is the main entry point for blimp_fly function.
--------------------------------------------------------------------------*/
void blimp_fly()
{
  int i, j;

  /*-------------------------------------------------------------------------
    Use magic to construct a flying dirigible in mid air.
  --------------------------------------------------------------------------*/
  cls();                            /* start with clear screen */

  for (i=0; i<=5; i++)              /* drop down five blank lines */
  {
    cout(0x0d);
    cout(0x0a);
  }

  /*-------------------------------------------------------------------------
    Build the fuselage and fill the thing with hydrogen gas.  No Smoking...
  --------------------------------------------------------------------------*/

  for (i=0; i<sizeof(blimp0)-1; i++)
    cout(blimp0[i]);
  cout(0x0d);
  cout(0x0a);

  for (i=0; i<sizeof(blimp1)-1; i++)
    cout(blimp1[i]);
  cout(0x0d);
  cout(0x0a);

  for (i=0; i<sizeof(blimp2)-1; i++)
    cout(blimp2[i]);
  cout(0x0d);
  cout(0x0a);

  for (i=0; i<sizeof(blimp3)-1; i++)
    cout(blimp3[i]);
  cout(0x0d);
  cout(0x0a);

  for (i=0; i<sizeof(blimp4)-1; i++)
    cout(blimp4[i]);
  cout(0x0d);
  cout(0x0a);

  for (i=0; i<sizeof(blimp5)-1; i++)
    cout(blimp5[i]);
  cout(0x0d);
  cout(0x0a);

  for (i=0; i<sizeof(blimp6)-1; i++)
    cout(blimp6[i]);
  cout(0x0d);
  cout(0x0a);

  for (i=0; i<sizeof(blimp7)-1; i++)
    cout(blimp7[i]);
  cout(0x0d);
  cout(0x0a);

  for (i=0; i<sizeof(blimp8)-1; i++)
    cout(blimp8[i]);
  cout(0x0d);
  cout(0x0a);

  for (i=0; i<sizeof(blimp9)-1; i++)
    cout(blimp9[i]);
  cout(0x0d);
  cout(0x0a);

  for (j=0; j<=61; j++)
    cout(brag_message[j]);

  for (i=0; i<=5; i++)              /* drop down five blank lines */
  {
    cout(0x0d);
    cout(0x0a);
  }

  /* Now pause after showing the blimp */
  pause();

  return;
}
