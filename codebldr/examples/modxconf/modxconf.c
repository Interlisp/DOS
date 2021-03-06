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

#if defined (XVERSION)
char title[]=
"Code Builder(TM) Kit Virtual Memory Configuration Utility   X026 \n \n";
#else
char title[]=
"Code Builder(TM) Kit Virtual Memory Configuration Utility   V1.1 \n \n";
#endif

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
#include "modxconf.h"                  /* defines & func prototypes          */

/******************************************************************************
*   Global variables defined during C startup
******************************************************************************/
extern _GDA  *_gda;         /* ptr to the gda                                */

/******************************************************************************
*   Global variables referenced within this file
******************************************************************************/
char        arg_filename[64+12+1];     /* cmd line arg copied to here for mod*/
int         cb_index;                  /* current index within 'cb_files[]'  */
char        cb_filename[64+12+1];      /* loooong path + filename            */
char        cb_files[6][20]=           /* list of tools files to modify      */
{
   "BIN\\DB32.EXE", "BIN\\IC32.EXE", "BIN\\IL32.EXE",
   "BIN\\LIB32.EXE", "BIN\\XMLOD", '\0'
};
BYTE           cb_tools;             /* TRUE if CB tools being modified    */
char           *cbpath;              /* value of CBPATH environment var    */
int            cpy_argc;             /* argc, argv of 'main()' globalized  */
char           **cpy_argv;
short          date_stamp;           /* orig date stamp of opened file      */
char           diskbuf[4096];        /* read/write 1st portion of a file   */
char           *filename;            /* ptr to current open filename       */
unsigned long  free_disk;            /* actual # of bytes avail on swap drv*/
unsigned long  free_mem;             /* amount of free ext mem upon entry  */
unsigned long  max_regionK;          /* max allowable region size (DOS)    */
_PROFILE       *prof;                /* ptr within 'diskbuf' to profile    */
int            rgn_handle;           /* file opened for region changes     */
char           *swap;                /* value of SWAP environment var      */
unsigned long  swap_size;            /* # of bytes in swap file (0=none)   */
short          time_stamp;           /* orig time stamp of opened file      */

static unsigned char char_attrib = BLUE;
static unsigned char bgnd_attrib = WHITE;
static unsigned char curr_row = 0;
static unsigned char curr_col = 0;

static unsigned char wind_top = 0;
static unsigned char wind_left = 0;
static unsigned char wind_bot = 24;
static unsigned char wind_right = 80;
static unsigned char wind_max = 80;

static unsigned char *outbuf;
static unsigned int scr_rows;
static unsigned int top_offset=0;
static unsigned int bot_offset=0;

static int get_numrows()
{
	union REGS inregs, outregs;

	inregs.x.ax=0x1130; /* Get Font Information */

	int86(0x10, &inregs, &outregs);
	return outregs.h.dl;
}

static set_textwindow(
unsigned char top,
unsigned char left,
unsigned char bottom,
unsigned char right
)
{
	wind_top = top-1;
	wind_left = left-1;
	wind_bot = bottom-1;
	wind_right = right-1;
}

static set_bkcolor(unsigned char col)
{
	bgnd_attrib = col;
}

static set_textcolor(unsigned char col)
{
	char_attrib = col;
}

static set_textposition_raw(unsigned char row, unsigned char col)
{
	union REGS inregs, outregs;

	inregs.h.ah=2; /* Set Cursor Position */
	inregs.h.dh=row+top_offset;
	inregs.h.dl=col;
	inregs.h.bh=0;

	int86(0x10, &inregs, &outregs);
	curr_row=row+top_offset;
	curr_col=col;
}

static set_textposition(unsigned char row, unsigned char col)
{
	unsigned char newrow;
	newrow = row + wind_top-1;
	
	set_textposition_raw(newrow, col+wind_left-1);
}

static out_text(unsigned char * msgbuf)
{
	union REGS inregs, outregs;
	unsigned char *outp;

	outp = outbuf;

	while(*msgbuf)
	{
		if(*msgbuf == '\n')
		{
			*outp++ = '\r';
			*outp++ = '\n';
			msgbuf++;
		}
		else
		{
			*outp++ = *msgbuf++;
		}
	}

	*outp++='\0';

	inregs.h.ah=0x13; /* Write String */
	inregs.h.al=1;
	inregs.x.cx=strlen(outbuf);
	inregs.h.bl=(bgnd_attrib<<4) | char_attrib;
	inregs.h.dh=curr_row;
	inregs.h.dl=curr_col;
	inregs.h.bh=0;
	inregs.w.ebp=(int)outbuf;
	int86(0x10, &inregs, &outregs);
}

static out_blanks(unsigned char count)
{
	union REGS inregs, outregs;

	inregs.h.ah=9; /* Write Attribute/Character  */
	inregs.h.al=' ';
	inregs.x.cx=count;
	inregs.h.bl=(bgnd_attrib<<4) | char_attrib;
	inregs.h.bh=0;
	int86(0x10, &inregs, &outregs);
}

static clear_screen(unsigned char wind)
{
	unsigned char row;
	static first_clear = TRUE;

	if(first_clear && (scr_rows > wind_bot))
	{
		for(row=wind_top; row<=scr_rows; row++)
		{
			set_textposition_raw(row,wind_left);
			out_blanks(wind_right-wind_left+1);
		}
		top_offset= scr_rows-wind_bot;
		bot_offset= (top_offset>>1) + (top_offset & 1);
		top_offset >>= 1;
		wind_bot = scr_rows;
	}
	else
	{
		for(row=wind_top; row<=wind_bot; row++)
		{
			set_textposition_raw(row,wind_left);
			out_blanks(wind_right-wind_left+1);
		}
	}
	first_clear = FALSE;
	set_textposition_raw(wind_top, wind_left);
}




/******************************************************************************
*  main()
******************************************************************************/
int   main(int argc, char** argv)
{
	static char version_str[] = 
#if defined (XVERSION)
	    "$CBVersion: Modxconf X026 (Memory Configuration Utility) $";
#else
	    "$CBVersion: Modxconf V1.1 (Memory Configuration Utility) $";
#endif



/******************************************************************************
*  Perform initialization, exit now if error
******************************************************************************/
   cpy_argc = argc;                    /* "globalize" & copy cmd line params */
   cpy_argv = argv;

   if (!Init())                        /* non-zero return means error - exit */
      return(1);

/******************************************************************************
*  Display helpful info (about regions, etc.) & program menu and process
******************************************************************************/
   Display_help();                     /* display the main static screen     */

   Main_menu();                        /* do the rest of the processing      */

   Cleanup();
/******************************************************************************
*  Exit
******************************************************************************/
   return(0);
}



/******************************************************************************
*  Abbrev_path(buffer, filename, max_size)
*
*  Copies 'filename' into buffer.  Ensures filename string (including path) is
*  not longer than 'max_size'.  If so, the path\filename is truncated (and
*  "..." inserted) to fit.
*
*  This routine assumes that "...\filename.ext" will ALWAYS fit in 'buffer'!
*
*  Input:
*     buffer      -  Ptr to char array to copy filename into (at least
*                       MIN_SIZE [currently 15] + 1 characters).
*     filename    -  Path/filename to copy (& possibly shorten).
*     max_size    -  Maximum number of characters (excludes '\0') to copy.
*
*  Output:
*     buffer      -  Path\filename as copied.
*  
*  Returns:
*     void
******************************************************************************/
void  Abbrev_path(char *buffer, char *filename, int max_size)
{
#define MIN_SIZE     (3+1+8+1+3)       /* "...\filename.ext" is min possible */


   strcpy(buffer, filename);
   if (strlen(filename) > max_size)
   {
      buffer[max_size - MIN_SIZE] = '\0'; /* stop at max spot possible       */
      strcat(buffer, "...");              /* now append ... & rtmost portion */
      strcat(buffer, strrchr(filename, '\\'));
   }

}




/******************************************************************************
*  Display_help()
*
*  Clears the entire (25 line) screen and sets the background to blue.
*  Displays several lines of fixed help text regarding "region size".
*
*  Input:
*     None
*
*  Output:
*     None
*
*  Returns:
*     void
******************************************************************************/
void  Display_help(void)
{
   char     msgbuf[180];             /* temporary 'sprintf()' buffer          */

   
   set_textcolor(LT_CYAN);
   sprintf(msgbuf, "%s\n", title);
   out_text((unsigned char *)msgbuf);
   set_textcolor(IWHITE);

   set_textposition(TEXT_ROW,TEXT_COL);

   out_text((unsigned char *)
"This program enables you to modify the region (virtual memory) size of the file\n"
"you specified on the command line, or of the Code Builder tools if you did not\n"
"specify a file.  You can also create a swap file, or resize an existing swap\n"
"file.  When executing under a DPMI host such as Windows(TM), the region size\n"
"determines the maximum amount of memory available to the program.\n\n"
"When the extender acts as the DPMI host, the maximum amount of virtual memory\n"
"available to the program is the largest of the following:\n"
"  o  The program's region size, less the amount of extended memory taken up by\n"
"     loading the program IF a swap file of sufficient size can be created;\n"
"  o  The amount of extended memory available after the program is loaded;\n"
"  o  The size of an existing swap file plus the amount of extended memory.");

}



/******************************************************************************
*  Display_live_info()
*
*  Gathers current statistics regarding region size, swap file size, etc.,
*  and displays the information.
*
*  Input (global):
*     filename    -  Ptr to currently opened file (CB tool or from cmd line).
*     free_mem    -  Amount of free extended memory at entry.
*     swap        -  Ptr to current value of the environment variable "SWAP".
*
*  Output:
*     free_disk    -  # of bytes available for a swap file (assuming current
*                          swap file, if any, is deleted).
*     max_regionK  -  Max allowable region size (available disk + mem, but
*                                    rounded down to nearest 256K) (in Kbytes).
*     swap_size    -  Size of current swap file, 0 if none.
*
*  Returns:
*     void
******************************************************************************/
void  Display_live_info(void)
{

   unsigned long        actual_used;   /* temp variable for 'free_disk' calc */
   struct diskfree_t    disk_size;     /* used to get swap drive statistics  */
   char                *file;          /* temp ptr to a string               */
   struct find_t        fileinfo;      /* used to get swap file information  */
   char                 fmt_buf[80];   /* temp sprintf fmt buffer            */
   unsigned long        one_cluster;   /* temp variable for 'free_disk' calc */
   char                 tempbuf[80];   /* temp string buffer                 */


/******************************************************************************
*  Create & clear window, display filename from cmd line or "CB tools" if none
******************************************************************************/
   set_textwindow(LIVE_INFO_TOP, LIVE_INFO_LEFT, LIVE_INFO_BOTTOM,
                     LIVE_INFO_RIGHT);
   set_textcolor(LT_CYAN);
   clear_screen(_GWINDOW);

   if (cb_tools)                       /* set to user filename or CB desc str*/
      file = "Code Builder tools.";
   else
   {
      if (strlen(filename) > FILE_NM_MAX) /* only have room for this much    */
      {
         Abbrev_path(tempbuf, filename, FILE_NM_MAX);
         file = tempbuf;
      }
      else 
         file = filename;
   }
   sprintf(fmt_buf, "File:  %s", file);
   out_text((unsigned char *)fmt_buf);

/******************************************************************************
*  Display current region size of target file, and free extended memory size
******************************************************************************/
   set_textposition(RGN_ROW, RGN_COL);
   sprintf(fmt_buf, "Region size:  %6luK", prof->pro_rgn/1024);
   out_text((unsigned char *)fmt_buf);

   set_textposition(EXT_MEM_ROW, EXT_MEM_COL);
   sprintf(fmt_buf, "Extended memory:  %luK", free_mem/1024);
   out_text((unsigned char *)fmt_buf);

/******************************************************************************
*  Get free disk space on swap drive and add size of current swap file (if any)
******************************************************************************/
   _dos_getdiskfree((unsigned)(toupper(swap[0])-'A')+1, &disk_size);
   free_disk = (unsigned long)disk_size.avail_clusters * disk_size.sectors_per_cluster *
               (unsigned long)disk_size.bytes_per_sector;

   if (_dos_findfirst(swap, _A_NORMAL, &fileinfo) == 0)  /* swap file exists */
   {
      swap_size = fileinfo.size;       /* calc size (rnd to nearest cluster) */
      one_cluster = disk_size.sectors_per_cluster * disk_size.bytes_per_sector;
      if ((swap_size % one_cluster) != 0)
         actual_used = ((swap_size / one_cluster) + 1) * one_cluster;
      else
         actual_used = swap_size;      /* evenly divisible by a cluster      */

      free_disk += actual_used;        /* add back in size of current file   */
   }
   else                                /* swap file doesn't exist            */
      swap_size = 0;

/******************************************************************************
*  Display swap file path & name as set by SWAP, or "No file" if none exists
******************************************************************************/
   Abbrev_path(tempbuf, swap, SWAP_FILE_MAX); /* trunc swap file at max   */
   set_textposition(SWAP_FILE_ROW, SWAP_FILE_COL);
   sprintf(fmt_buf, "Swap file:  %s", tempbuf);
   out_text((unsigned char *)fmt_buf);

   if (swap_size == 0)
      strcpy(tempbuf, "No file");
   else
      sprintf(tempbuf, "%luK", swap_size/1024);

   set_textposition(SWAP_SIZE_ROW, SWAP_SIZE_COL);
   sprintf(fmt_buf, "Swap size:  %s", tempbuf);
   out_text((unsigned char *)fmt_buf);

/******************************************************************************
*  Display max swap file size and max region size (max swap + free ext mem)
******************************************************************************/
   set_textposition(MAX_SWAP_ROW, MAX_SWAP_COL);
   sprintf(fmt_buf, "Max swap size:  %6luK", free_disk /1024);
   out_text((unsigned char *)fmt_buf);

   max_regionK = (free_mem + free_disk) / (unsigned long)1024;
                                                    /* round down to closest 256K incremnt*/
    max_regionK = (max_regionK / 256) * 256;
   set_textposition(MAX_RGN_ROW, MAX_RGN_COL);
   sprintf(fmt_buf, "Max region size:  %6luK", max_regionK);
   out_text((unsigned char *)fmt_buf);

}



/******************************************************************************
*  Error(msg)
*
*  Defines & clears a window and displays an error message in that window.
*  Then displays a "Press any key ..." message & waits for a key to be pressed.
*
*  Input:
*     msg   -  Error message to be displayed.  May contain embedded "\n"s for
*              multiple lines (may have up to the window height - 1 lines).
*  Output:
*     None
*
*  Returns:
*     void
******************************************************************************/
void  Error(char *msg)
{
   char                 fmt_buf[160];  /* temp sprintf fmt buffer            */


   set_textwindow(ERR_TOP, ERR_LEFT, ERR_BOTTOM, ERR_RIGHT);
   set_textcolor(YELLOW);
   clear_screen(_GWINDOW);

   sprintf(fmt_buf, "%s", msg);        /* display caller's error message     */
   out_text((unsigned char *)fmt_buf);
                                       /* center & display "Press..." msg    */
   set_textposition(ERR_BOTTOM-ERR_TOP, (ERR_RIGHT-ERR_LEFT)/2 - 15);
   sprintf(fmt_buf, "Press any key to continue....", msg);
   out_text((unsigned char *)fmt_buf);

   fflush(stdin);
   if (getch() == 0)                   /* wait for any char; get both parts  */
      getch();

   return;

}


/******************************************************************************
*  Cleanup()
******************************************************************************/
void Cleanup(void)
{
   set_textwindow(DONE_ROW,DONE_COL,DONE_ROW+bot_offset,MENU_RIGHT);
   set_textposition(FILE_ROW+bot_offset, FILE_COL);
   _dos_freemem((unsigned)outbuf);
}


/******************************************************************************
*  Init()
*
*  First-time initialization upon program entry.
*
*  To ensure the value read from the inter-task communications area was passed
*  to it by MODXCONF, a check of a "signature" value is performed.  If the
*  check fails, no further processing is performed.  If it passes, the
*  signature is zeroed to prevent re-execution without running MODXCONF.
*
*  The current values of the environment variables 'SWAP' & 'CBPATH' are
*  retrieved.  If 'SWAP' is not defined, a default value is used instead.
*
*  Validate the command line parameters, opening the file specified (or the
*  1st of the tools list if no file was given).
*
*  Input:
*     None
*
*  Output:
*     cb_index -  0 if 'cb_tools' is TRUE, else undefined.
*     cbpath   -  Ptr to current value of the environment variable "CBPATH".
*     cb_tools -  TRUE if CB tools are to be changed, FALSE if user file.
*     filename -  Ptr to opened target file name.
*     swap     -  Ptr to current value of the environment variable "SWAP".
*
*  Returns:
*     TRUE  -  Initialization successful.
*     FALSE -  Could not open a file for modification.
******************************************************************************/
int   Init(void)
{
   char        fmt_buf[800];           /* temp sprintf fmt buffer            */
   char        prog_name[20];          /* this program's (current) name      */
   int         result = TRUE;          /* return value (assume OK)           */
   char       *string;                 /* temp ptr to string                 */



/******************************************************************************
*  Allocat buffer required for out_text.
******************************************************************************/
   if(_dos_allocmem((25*80) >> 4 ,&outbuf))
	   result=FALSE;                   /* Could not allocate DOS Memory.     */

/******************************************************************************
*  Clear screen to blue background, intense white foreground
******************************************************************************/
   scr_rows=get_numrows();
   set_bkcolor(BLUE);
   set_textcolor(IWHITE);
   set_textwindow(1, 1, 25, 80);
   clear_screen(_GWINDOW);

/******************************************************************************
*  Copy program's name from command line arg to 'prog_name'
******************************************************************************/
                                       /* copy prog's name for modification  */
   string = strrchr(cpy_argv[0], (int)'\\'); /* find last path separator char*/
   if(!string) string = cpy_argv[0];
   else string++;
   strcpy(prog_name, string);        /* copy, discarding path              */
   string = strchr(prog_name, (int)'.');/* find extension & remove it        */
   *string = '\0';

/******************************************************************************
*  Determine max amount of extended memory in system
******************************************************************************/
    free_mem = _gda->gda_tstg - (unsigned)_gda->gda_hbase;
    if ((free_mem % 1024) != 0)
        free_mem = ((free_mem/1024) + 1) * 1024;    /* force size to be 1K incrmt*/

/******************************************************************************
*  Get current value of the CBPATH environment variable & store in 'cbpath'
******************************************************************************/
   cbpath = getenv("CBPATH");          /* get current value of CBPATH env var*/

/******************************************************************************
*  Parse the command line parameters; display usage instructions if error
******************************************************************************/
   switch (cpy_argc)
   {
      case 2:                          /* one prog name specified            */
         strcpy(arg_filename, cpy_argv[1]);  /* copy in case of append       */
         filename = arg_filename;
         if (strchr(filename, '.') == NULL)  /* if no ext, append .EXE       */
            strcat(filename, ".exe");
         if (! Open_filename())        /* validate prog name & read region sz*/
            goto bad_arg;
         cb_tools = FALSE;
         break;

      case 1:                          /* no prog name - CB tools is deflt   */
         if (cbpath == NULL)           /* not Code Builder developmnt environ*/
         {                             /* and no program name specified - err*/
            result = FALSE;
            sprintf(fmt_buf,
                   "\nMissing program name.\n"
                   "\nUsage:  %s progname\n"
                   "        Where 'progname' specifies the path and filename "
                   "of the file whose\n"
                   "        region is to be examined/changed.\n\n",
                   prog_name);
            out_text((unsigned char *)fmt_buf);
         }
         else                          /* insert path in front of CB file    */
         {
            cb_index = 0;              /* start with 0th file in tools list  */
            if (! Open_CB_name())      /* Read region sz of 1 of the tools   */
               goto bad_arg;
            cb_tools = TRUE;
         }
         break;

      default:
         sprintf(fmt_buf, "Too many arguments.\n");
         out_text((unsigned char *)fmt_buf);
bad_arg:
         sprintf(fmt_buf, "\n\nUsage:  %s [progname]\n"
                "        Where 'progname' specifies the path and filename "
                "of the file whose\n"
                "        region is to be examined/changed (default = Code "
                "Builder tools).\n\n", prog_name);
         out_text((unsigned char *)fmt_buf);
         
         result = FALSE;
         break;
   }

/******************************************************************************
*  Get current value of the SWAP env var; default to profile setting if none
******************************************************************************/
   if (result)                         /* if profile was successfully opened */
   {
      swap = getenv("SWAP");           /* get current value of SWAP env var  */

      if (swap == NULL)
         swap = prof->pro_swp;         /* set to default swap string if none */
   }

   return(result);
}



/******************************************************************************
*  Main_menu()
*
*  Loops until "Quit" selected, updating the current region size, swap file
*  size, etc., and allowing selections from the menu.  Processes the menu
*  item selected.
*
*  Input:
*     None
*
*  Output:
*     None
*
*  Returns:
*     void
******************************************************************************/
void  Main_menu(void)
{
   char     choice;
   char     fmt_buf[500];              /* temp sprintf fmt buffer            */
   BYTE     quit = FALSE;


   do
   {
      Display_live_info();             /* display filename & region size     */
   
      set_textwindow(MENU_TOP, MENU_LEFT, MENU_BOTTOM, MENU_RIGHT);
      set_textcolor(LT_GREEN);
      clear_screen(_GWINDOW);
   
      sprintf(fmt_buf,
           "        M - Modify region size."
         "\n        C - Create/resize swap file for stand-alone hosting."
         "\n        Q - Quit.\n"
           "        Press choice:  ");
      out_text((unsigned char *)fmt_buf);

      fflush(stdin);
      choice = toupper(getch());
      if (choice == (char)0)           /* pressed a func key instead!        */
         getch();

      switch (choice)
      {
         case 'Q':
            quit = TRUE;               /* finished with program - exit       */
            break;

         case 'M':
            Mod_dos();                 /* modify region for stand-alone DOS   */
            break;

         case 'C':
            Mod_swap();                /* modify/create swap file            */
            break;
      }
   }
   while (!quit);

   set_textcolor(IWHITE);
   clear_screen(_GWINDOW);             /* erase the menu for cleanliness     */
   return;
}


/******************************************************************************
*  Mod_dos()
*
*  Allows modification to the region size of the target file(s) assuming
*  execution under stand-alone DOS.  Ensures that adequate resources are
*  available if the files being modified are the tools.
*
*    The "suggested" region size is calculated by adding the amount of extended
*  memory + a percentage (SWAP_DRIVE_PCT) of the available swap drive space.
*
*  Exits when the region size has been changed, or when "Quit" selected.
*
*  Input (global):
*     cb_tools       -  TRUE if CB tools are to be changed, FALSE if user file.
*     free_disk      -  # of bytes available for a swap file (assuming current
*                          swap file, if any, is deleted).
*     free_mem       -  Free extended memory upon entry.
*        max_regionK        -  Max allowable region size (for manual entry).
*
*  Output:
*     None
*
*  Returns:
*     void
******************************************************************************/
void  Mod_dos()
{
   char     choice;                    /* menu selection                     */
   char     fmt_buf[6*80];             /* temp sprintf format buffer         */
   unsigned long     min_regionK;      /* min allowable region (2MB if tools)*/
   unsigned long     new_regionK;      /* new value for region being modified*/
   unsigned long     sug_regionK;      /* suggested region size              */
   BYTE     quit;                      /* FALSE until time to exit           */


   quit = FALSE;                       /* true if error or selection made    */
    
    if (_gda->gda_env & GDA_DPMI)
        sug_regionK = 3072; /* 3 Megabytes assumed for Windows */
    else
        sug_regionK = (free_mem + ((free_disk /100) * SWAP_DRIVE_PCT)) / 1024;

/******************************************************************************
*  If Code Builder tools are being changed, ensure resources are adequate
******************************************************************************/
   if (cb_tools)
   {
      min_regionK = 2048;              /* all values in K for region mods    */
      if (free_mem < 0x100000)
      {
         Error("        You must have at least 1 MB of free extended memory "
                "to execute\n        the Intel Code Builder tools.");
         quit = TRUE;
      }
      else if (max_regionK < 2048)
      {
         Error("        You must have at least 2 MB combined of free extended "
               "memory and\n        swap drive space to execute the Intel Code "
               "Builder tools.\n");
         quit = TRUE;
      }
   }
   else
      min_regionK = 0;                 /* allow region size = 0 if not tools */

/******************************************************************************
*  Process menu selections until quit selected or region successfully changed
******************************************************************************/
   while (! quit)
   {
      set_textwindow(MENU_TOP, MENU_LEFT, MENU_BOTTOM, MENU_RIGHT);
      set_textcolor(LT_GREEN);
      clear_screen(_GWINDOW);          /* erase the prev menu                */

      sprintf(fmt_buf,
           "        E - Enter a new region size."
         "\n        A - Accept suggested region size of %dK."
         "\n"
         "\n        Q - Quit to main menu."
         "\n        Press choice:  ", sug_regionK);
      out_text((unsigned char *)fmt_buf);

      fflush(stdin);
      choice = toupper(getch());
      if (choice == (char)0)           /* pressed a func key instead!        */
         getch();

      switch (choice)
      {
         case 'Q':
            quit = TRUE;
            break;

         case 'E':                     /* edit to specific value             */
            if (_gda->gda_env & GDA_DPMI)
            {
                /* Under Windows, do not restrict the maximum region size */

                new_regionK = Value_K(0, min_regionK, "region size");
            }
            else
            {
                new_regionK = Value_K(max_regionK, min_regionK, "region size");
            }
            if (new_regionK != -1)
            {
               Modify_files(new_regionK);
               quit = TRUE;            /* region successfully changed;  done */
            }
            break;

         case 'A':                     /* set to suggested value             */
            Modify_files(sug_regionK);
            quit = TRUE;               /* region successfully changed;  done */
            break;
      }
   }

   return;
}



/******************************************************************************
*  Mod_swap()
*
*  Allows modification of the size of the swap file.  The only restriction
*  placed on the value is that it must be <= than the amount of free space
*  on the swap drive.
*
*  A suggested swap file size is given by taking the current region size minus
*  the amount of extended memory, and adding SWAP_MIN_K to the result.
*
*  Exits when the size has been changed, or when "Quit" selected.
*
*  Input (global):
*     swap_size   -  Size of current swap file, 0 if none.
*
*  Output:
*     None
*
*  Returns:
*     void
******************************************************************************/
void  Mod_swap()
{
   char     choice;                /* menu selection                     */
   char     fmt_buf[500];          /* temp sprintf fmt buffer            */
   unsigned long     new_swapK;    /* manually entered new swap file sz  */
   unsigned long     sug_swapK;    /* suggested value for swap file sz   */
   BYTE     quit;                  /* FALSE until time to exit           */


/******************************************************************************
*  Calc suggested swap file size
******************************************************************************/
    if (prof->pro_rgn > free_mem)
        /* add "arbitrary" value to difference*/
        sug_swapK = ((prof->pro_rgn - free_mem) / 1024) + SWAP_MIN_K;
    else        /* region size was < amount free mem  */
        sug_swapK = SWAP_MIN_K;

    if (sug_swapK > free_disk/1024)
        sug_swapK = free_disk/1024;

/******************************************************************************
*  Process menu selections until quit selected or swap sz successfully changed
******************************************************************************/
   quit = FALSE;
   while (! quit)
   {
      set_textwindow(MENU_TOP, MENU_LEFT, MENU_BOTTOM, MENU_RIGHT);
      set_textcolor(LT_GREEN);
      clear_screen(_GWINDOW);          /* erase any prev text                */

      sprintf(fmt_buf,
           "        E - Enter a specific size for the swap file."
         "\n        A - Accept suggested value of %dK based on region size."
         "\n"
         "\n        Q - Quit to main menu."
         "\n        Press choice:  ", sug_swapK);
      out_text((unsigned char *)fmt_buf);

      fflush(stdin);
      choice = toupper(getch());
      if (choice == (char)0)           /* pressed a func key instead!       */
         getch();

      switch (choice)
      {
         case 'Q':                     /* quit back to main menu            */
            quit = TRUE;
            break;

         case 'E':                     /* enter a specific value            */
            new_swapK = Value_K(free_disk/1024, 0, "swap file size");
            if (new_swapK != -1)
            {
               Write_swap(new_swapK);
               quit = TRUE;            /* swap file successfully changed;done*/
            }
            break;

         case 'A':                     /* suggested value                    */
            Write_swap(sug_swapK);
            quit = TRUE;               /* swap changed; done                 */
            break;
      }
   }

   return;
}

/******************************************************************************
*  Modify_files(new_rgn)
*
*  Modifies the region size of the target file(s) to 'new_rgn' * 1024.
*
*  Input:
*     new_rgn     -  Value (in Kbytes) of new region size.
*
*  Input (global):
*     cb_index     -  Set to current index within 'cb_files[]' of file to open.
*                     (Only applicable if 'cb_tools' is TRUE.)
*     cb_tools     -  TRUE if CB tools are to be changed, FALSE if user file.
*     rgn_handle  -  Handle of file opened for region reading/changing.
*
*  Output:
*     None
*
*  Returns:
*     void
******************************************************************************/
void  Modify_files(unsigned long new_rgn)
{


/******************************************************************************
*  If a single user-app is being modified, just do it
******************************************************************************/
   if (!cb_tools)
      Write_region(new_rgn);           /* modify region size of open file    */

/******************************************************************************
*  Code Builder tools being changed - Loop to change all files in list
******************************************************************************/
   else
   {
      while (Write_region(new_rgn))    /* update previously opened file      */
      {
         close(rgn_handle);            /* only need 1 file open at a time    */
         if (cb_files[++cb_index][0] == '\0')
            break;                     /* adv to next file; exit when done   */
         if (! Open_CB_name())         /* open next file                     */
            break;                     /* should never happen, but ...       */
      }
      cb_index = 0;                    /* re-open 0th file                   */
      Open_CB_name();
   }
}




/******************************************************************************
*  Open_CB_name()
*
*  Inserts 'cbpath' in front of the current Code Builder tools filename and
*  calls 'Open_filename()' to open the file.
*
*  Input (global):
*     cbpath   -  Ptr to current value of the environment variable "CBPATH".
*     cb_index -  Set to current index within 'cb_files[]' of file to open.
*
*  Output:
*     cb_filename -  Opened target file name.
*     filename    -  Ptr to opened target file name ('cb_filename').
*
*  Returns:
*     TRUE  -  Initialization successful.
*     FALSE -  Could not open the 1st Code Builder file for modification.
******************************************************************************/
int   Open_CB_name(void)
{
   char  last_char;                    /* ending char of CBPATH              */
   int   result = TRUE;                /* return value (assume OK)           */


   strcpy(cb_filename, cbpath);
   last_char = cbpath[strlen(cbpath)-1]; /* get ending char of CBPATH var    */
   if ((last_char != '\\') && (last_char != '/'))
      strcat(cb_filename, "\\");       /* need to insert missing '\' b4 fnm  */
   strcat(cb_filename, cb_files[cb_index]);
   filename = cb_filename;
   result = Open_filename();           /* attempt to open & read region size */

   return(result);
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
*  Value_K(max, min, desc)
*
*  Creates and clears a window and displays a prompt for data entry of a
*  value.  The value is assumed to be entered in K bytes, unless appended by
*  an 'M'.  In either case, it will be returned as K bytes.  (In other words,
*  if the value is appended by 'M', it will be returned * 1024).
*
*  If no value is entered (i.e., <return> is pressed), "quit" is assumed.  If
*  a value is entered, it is bounds-checked against max & min (or just min if
*  max is 0).  If the value is out of bounds, an error message is displayed
*  and input will be attempted again.
*
*  Input:
*     max   -  The maximum allowable value (0 = no maximum).
*     min   -  The minimum allowable value.
*     desc  -  A VERY short description of the value being entered.
*
*  Output:
*     None
*
*  Returns:
*     New value entered (assumed to be Kbytes) or -1 if quit.
******************************************************************************/
unsigned long  Value_K(unsigned long max, unsigned long min, char *desc)
{
   char  buffer[80];
   char  fmt_buf[200];              /* temp sprintf fmt buffer            */
   unsigned long  new_value;
   char  suffix;
   char  value_string[80];
   BYTE  waiting;


   waiting = TRUE;
   while (waiting)
   {
      set_textwindow(VALUE_TOP, VALUE_LEFT, VALUE_BOTTOM, VALUE_RIGHT);
      set_textcolor(IWHITE);
      clear_screen(_GWINDOW);

      sprintf(fmt_buf, "        Enter a new value for %s\n"
             "        (default is in K bytes; append M for Megabytes): ",
            desc);
      out_text((unsigned char *)fmt_buf);
      value_string[0] = '\0';
      scanf("%[+-9Mm]", value_string);

      if (value_string[0] == '\0')     /* invalid or hit return = "ESC"      */
      {
         new_value = -1;               
         waiting = FALSE;
      }
      else
      {
         suffix = toupper(value_string[strlen(value_string) - 1]); /* last ch*/
         sscanf(value_string, "%lu", &new_value);  /* convert string to unsigned long */
         if (suffix == 'M')
            new_value *= 1024;         /* 1024K = 1M                         */

         if (max == 0)
         {
            if (new_value < min)
            {
               sprintf(buffer, "        New %s must be greater than %lu",
                                desc, min);
               Error(buffer);
            }
            else
               waiting = FALSE;
         }
         else if ((new_value < min) || (new_value > max))
         {
            sprintf(buffer, "        New %s must be between %lu and %lu",
                            desc, min, max);
            Error(buffer);
         }
         else
            waiting = FALSE;
      }
   }
   return(new_value);
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
int   Write_region(unsigned long new_rgn)
{
   char          fmt_buf[180];
   union REGS  regs;
   int           result = TRUE;


   prof->pro_rgn = new_rgn * 1024;     /* modify the opened file's region    */
                                       /* seek to region value on disk       */
   lseek(rgn_handle, (unsigned long)((char *)&prof->pro_rgn - diskbuf), SEEK_SET);
                                       /* write region value to disk         */
   if (write(rgn_handle, &prof->pro_rgn, sizeof(prof->pro_rgn)) !=
         sizeof(prof->pro_rgn))
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



/******************************************************************************
*  Write_swap(new_swapK)
*
*  Opens (or creates) the swap path\file as defined by 'swap', seeks to the
*  new size, and writes out 0 bytes to this location (which extends the file
*  to the current file position).
*
*  The file is opened using "truncate" to allow sizing to any value.  If the
*  open is not successful, it is most likely due to an invalid value for the
*  SWAP environment variable.
*
*  Input:
*     new_swapK  -  Value (in Kbytes) of new swap file size.
*
*  Input (global):
*     swap       -  Ptr to current value of the environment variable "SWAP".
*
*  Output:
*     None
*
*  Returns:
*     TRUE if swap file was successfully re-sized.
*     FALSE if error occurred.
******************************************************************************/
int   Write_swap(unsigned long new_swapK)
{
   char          fmt_buf[180];
   union REGS  regs;
   int           result = TRUE;
   int           seek_byte;
   int           swap_handle;


/******************************************************************************
*     Open the swap file; truncate it so starts from 0
******************************************************************************/
   swap_handle = open(swap, O_BINARY | O_TRUNC | O_CREAT | O_RDWR, S_IREAD |
                        S_IWRITE);
   if (swap_handle == -1)
   {
      result = FALSE;
      Error("        Unable to open swap file - Invalid 'SWAP' environment "
            "variable?");
   }
   else
   {
/******************************************************************************
*     Clear the menu area & display "writing..." message
******************************************************************************/
      set_textwindow(MENU_TOP, MENU_LEFT, MENU_BOTTOM, MENU_RIGHT);
      set_textcolor(LT_GREEN);
      clear_screen(_GWINDOW);          /* erase any prev text                */
      sprintf(fmt_buf, "        Writing swap file ....\n");
      out_text((unsigned char *)fmt_buf);

/******************************************************************************
*     Seek to 1 byte past the last byte to be written
******************************************************************************/
      seek_byte = (new_swapK * 1024); /* should NOT be -1                     */
      if (seek_byte < 0)
         seek_byte = 0;

        regs.h.ah = 0x42;              /* seek to size desired                */
        regs.h.al = 0;                            
        regs.x.bx = swap_handle;            
        regs.x.cx = seek_byte >> 16;
        regs.x.dx = seek_byte & 0xffff;
        intdos(&regs, &regs);

/******************************************************************************
*     Write out 0 bytes; establishes new file size as current file pos'n - 1
******************************************************************************/
        regs.h.ah = 0x40;              /* (use DOS request rather than C lib */
        regs.x.bx = swap_handle;       /*  since C lib zero-fills)           */
        regs.x.cx = 0;                 /* extend file to current file pos'n  */
        regs.w.edx = (unsigned)fmt_buf;
        intdos(&regs, &regs);
        if (regs.x.cflag)
      {
         result = FALSE;
         Error("        Unable to set swap file to this size");
      }

      close(swap_handle);
   }
   return(result);
}
