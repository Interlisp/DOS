/*******************************************************************************
*  MODXCONF.C
*
*  This program displays information pertaining to region size & swap file
*  size.  It allows the region size of the target program to be modified, and
*  also can create a swap file of a specific size.
*
*  If a program name is given on the command line, it will be the target file
*  whose region size will be examined/modified.  If no program is specified,
*  the Code Builder tools will be the target files.  (For the latter case,
*  however, the CBPATH environment variable must be defined.)
*
*  A general description of "region size" and its meaning under stand-alone DOS
*  and Windows(TM) is displayed, followed by region-related current information
*  (e.g., current region size, swap file size, maximum values, etc.).
*
*  A menu allows the user to select an item to perform.  Based on this
*  selection, the region size can be modified, or a swap file created/resized.
*  The region size of the Code Builder tools has a minimum limit of 2MB.
*  Additionally, under stand-alone DOS, at least 1MB of free extended memory
*  is required to change the region size.  The region size of other programs
*  may be changed without restrictions, except under stand-alone DOS a value
*  greater than the current available resources is not allowed.
*
*  The target file (or the first file of the Code Builder tools target list) is
*  opened during initialization, and kept open for the duration of execution.
*  
*  Routines within this file are arranged alphabetically, except for 'main()'
*  which appears first.  Also, all routines begin with an upper case letter,
*  except again for 'main()', and use underscores to separate words.
*  
*  Input/Output shown for each module applies to those variables explicitely
*  used or set ONLY within that routine.  In other words, if routine A calls
*  routine B, variables used or set by routine B are not listed in routine A's
*  input/output list.
*  
*  The current program structure is as follows:
*     main        
*        Init
*           Open_filename              /* user filename on cmd line - open it*
*           Open_CB_name               /* no cmd line file; open all CB tools*
*              Open_filename
*        Display_help                  /* display static info on region size *
*        Main_menu
*           Display_live_info          /* display current region, swap stats *
*           Mod_dos                    /* Modify region size under DOS       *
*              Error                   /* (only if error - display err msg)  *
*              Value_K                 /* enter specific value; ret as Kb    *
*                 Error
*              Modify_files            /* modify region of target file(s)    *
*                 Write_region         /* change region of current open file *
*                 Open_CB_name         /* if CB tools are the target files   *
*           Mod_swap                   /* Modify swap file size              *
*              Value_K
*                 Error
*              Write_swap              /* create or resize to value given    *
*  
*  
*  Include file MODXCONF.H contains the defines which determine screen text
*  positioning.
*
*  Date: 26 Feb 91
******************************************************************************/


/******************************************************************************
*  Include files
******************************************************************************/
#include <ctype.h>
#include <dos.h>
#include <extender.h>
#include <fcntl.h>
#include <io.h>
#include <graph.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys\types.h>
#include <sys\stat.h>
#include "medconf.h"                  /* defines & func prototypes          */

short          date_stamp;           /* orig date stamp of opened file      */
char           diskbuf[4096];        /* read/write 1st portion of a file   */
char           filename[]="emul.exe";            /* ptr to current open filename       */
_PROFILE       *prof;                /* ptr within 'diskbuf' to profile    */
int            rgn_handle;           /* file opened for region changes     */
short          time_stamp;           /* orig time stamp of opened file      */


/******************************************************************************
*  main()
******************************************************************************/
int   main(int argc, char** argv)
{


/******************************************************************************
*  Perform initialization, exit now if error
******************************************************************************/

   if (!Open_filename())            /* non-zero return means error - exit */
      return(1);

/******************************************************************************
*  Display helpful info (about regions, etc.) & program menu and process
******************************************************************************/

      Write_region(argv[0]);           /* modify region size of open file    */
	close(rgn_handle);
/******************************************************************************
*  Exit
******************************************************************************/
   return(0);
}




/******************************************************************************
*  Open_filename()
*
*  Opens the current filename and reads the beginning portion into 'diskbuf'.
*  Sets 'prof' to point to start of profile within 'diskbuf'.
*
*  Input (global):
*     filename    -  Ptr to filename string (including path) of file to open.
*
*  Output:
*        date_stamp    -  Date stamp of opened file.
*     diskbuf     -  Beginning portion read from the opened file.
*     prof        -  Ptr (within 'diskbuf') to open file's profile.
*     rgn_handle  -  Handle of file opened for region reading/changing.
*        time_stamp    -  Time stamp of opened file.
*
*  Returns:
*     TRUE  -  Profile read successfully.
*     FALSE -  Couldn't open the file for read/write or couldn't find profile.
******************************************************************************/
int   Open_filename(void)
{
   char         *buf;
   char          fmt_buf[256];
   char         *p;                      /* temp ptr within 'diskbuf'          */
   union REGS  regs;
   int           retval = TRUE;


/******************************************************************************
*  Open the file and read the beginning portion (contains the profile)
******************************************************************************/
   if ((rgn_handle = open(filename, O_BINARY | O_RDWR)) == -1)
   {
      sprintf(fmt_buf, "Unable to open '%s' \n    for reading/writing: %s",
                    filename, _strerror(NULL));
      out_text((unsigned char *)fmt_buf);
      retval = FALSE;
      goto exit_open;
   }

   if (read(rgn_handle, diskbuf, sizeof(diskbuf)) != sizeof(diskbuf))
   {
      sprintf(fmt_buf, "Unable to read profile from '%s'.\n", filename);
      out_text((unsigned char *)fmt_buf);
      close(rgn_handle);
      retval = FALSE;
      goto exit_open;
   }

/******************************************************************************
*  Search for the profile in the portion just read
******************************************************************************/
   buf = diskbuf;                      /* init ptr to start of area just rd  */
   prof = (_PROFILE *)0;               /* init to "no profile found"         */

   do
   {
      if ((p = memchr(buf, 'P', &diskbuf[sizeof(diskbuf)-1]-buf)) == NULL)
      {
         sprintf(fmt_buf, "File '%s' is not an extended DOS application.\n", filename);
         out_text((unsigned char *)fmt_buf);
         close(rgn_handle);
         retval = FALSE;
         goto exit_open;
      }

      if (memcmp(prof = (_PROFILE *)p, "PRO ", sizeof(prof->pro_eye)) == 0)
      {
         if (memcmp(prof->pro_rgn_eye, "RGN=", sizeof(prof->pro_rgn_eye)) == 0)
            break;                     /* one extra check doesn't hurt!       */
      }
      buf = p+1;
   }
   while(&diskbuf[sizeof(diskbuf)-1]-buf > 0);

   if (prof == (_PROFILE *)0)
   {
      sprintf(fmt_buf, "File '%s' does not have a profile.\n", filename);
      out_text((unsigned char *)fmt_buf);
      close(rgn_handle);
      retval = FALSE;
      goto exit_open;
   }

/******************************************************************************
*  Get & save file date/time so if we modify file we can put back orig stamp
******************************************************************************/
    regs.x.ax = 0x5700;                        /* get file date/time         */
    regs.x.bx = rgn_handle;
    intdos(&regs, &regs);                    /* get date/time in cx & dx     */
    time_stamp = regs.x.cx;
    date_stamp = regs.x.dx;

/******************************************************************************
*  Exit 'Open_filename()'
******************************************************************************/
exit_open:
   if (!retval)                        /* error msg displayed - wait for key */
   {
      sprintf(fmt_buf, "\n\n\n           Press any key to continue .... ");
      out_text((unsigned char *)fmt_buf);
      fflush(stdin);
      if (getch() == 0)
         getch();
   }
   return(retval);
}


/******************************************************************************
*  Write_region(new_rgn)
*
*  Changes the region  pointed to by 'prof' to 'new_rgn' and writes out this
*  value to the opened file.  Updates the time/date stamp BACK to its original
*  value (so this little change doesn't get reflected).
*
*  Input:
*     new_rgn     -  Value (in Kbytes) of new region size.
*
*  Input (global):
*     date_stamp    -  Date stamp of opened file.
*     prof          -  Ptr (within 'diskbuf') to open file's profile.
*     rgn_handle    -  Handle of file opened for region modification.
*     time_stamp    -  Time stamp of opened file.
*
*
******************************************************************************/
int   Write_region(char * new_rgn)
{
   char          fmt_buf[180];
   union REGS  regs;
   int           result = TRUE;

   strcpy(&(prof->pro_swp), new_rgn);
   

   lseek(rgn_handle, (unsigned long)((char *)&prof->pro_swp - diskbuf), SEEK_SET);
                                       /* write region value to disk         */
   if (write(rgn_handle, &prof->pro_swp, sizeof(prof->pro_swp)) !=
         sizeof(prof->pro_swp))
   {
      sprintf(fmt_buf,"        Error while trying to write changes to '%s'.\n",
               filename);
      Error(fmt_buf);
      result = FALSE;
   }

    regs.x.ax = 0x5701;         /* set date/time stamp back to orig      */
    regs.x.bx = rgn_handle;
    regs.x.cx = time_stamp;
    regs.x.dx = date_stamp;
    intdos(&regs, &regs);       /* "change" to original values          */

   return(result);
}



