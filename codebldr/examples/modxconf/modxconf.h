/******************************************************************************
*                                                                      
*      Copyright 1991 Intel Corporation.
*      This source is intended for your benefit in developing applications
*      using the Intel 386(TM) / 486(TM) C Code Builder (TM) Kit.  Intel
*      hereby grants you permission to modify and incorporate it as needed.
*                                                                      
*******************************************************************************
*    MODXCONF.H
*
*    Header file of functions & defines for MODXCONF.C.
*
*    Date:    26 Feb 91
******************************************************************************/

/******************************************************************************
*    Defines
******************************************************************************/
#define BYTE            unsigned char
#define FALSE           0
#define TRUE            1

#define MENU_QUIT       0
#define MENU_MOD_NO_WIN 1
#define MENU_MOD_WIN    2
#define MENU_MOD_SWAP   3

#define SWAP_DRIVE_PCT  80   /* % of free space; calc of sug rgn size */
#define SWAP_MIN_K      1024 /* used in calc of sug swap file size    */

/******************************************************************************
*    Standard ega/vga palette color defines
******************************************************************************/
#define BLACK      0
#define BLUE       1
#define GREEN      2
#define CYAN       3
#define RED        4
#define MAGENTA    5
#define BROWN      6
#define WHITE      7
#define GRAY       8
#define LT_BLUE    9
#define LT_GREEN   10
#define LT_CYAN    11
#define LT_RED     12
#define LT_MAGENTA 13
#define YELLOW     14
#define IWHITE     15

/******************************************************************************
*
******************************************************************************/
#define MENU_TOP         21
#define MENU_LEFT        1
#define MENU_BOTTOM      25
#define MENU_RIGHT       80

#define ERR_TOP          21
#define ERR_LEFT         1
#define ERR_BOTTOM       25
#define ERR_RIGHT        80

#define VALUE_TOP        21
#define VALUE_LEFT       1
#define VALUE_BOTTOM     25
#define VALUE_RIGHT      80

#define LIVE_INFO_TOP    17
#define LIVE_INFO_LEFT   1
#define LIVE_INFO_BOTTOM 19
#define LIVE_INFO_RIGHT  80

#define FILE_ROW         1
#define FILE_COL         1
#define FILE_NM_MAX      (RGN_COL-9) /* max # of path\filename chars          */
                                     /* (-9 = room for 2 spcs and "File:  "   */

#define RGN_ROW          (FILE_ROW)  /* "Region size: "                       */
#define RGN_COL          59

#define EXT_MEM_ROW      (FILE_ROW+1)/* "Extended memory:  "                  */
#define EXT_MEM_COL      1

#define MAX_RGN_ROW      (EXT_MEM_ROW)/* "Max region size:  "                 */
#define MAX_RGN_COL      (RGN_COL-4)  /* back up, leaving room for "Max "     */

#define SWAP_FILE_ROW    (FILE_ROW+2) /* "Swap file:  "                       */
#define SWAP_FILE_COL    1
#define SWAP_FILE_MAX    (SWAP_SIZE_COL-14) /* rm for 2 spcs & "Swap file:  " */

#define SWAP_SIZE_ROW    (SWAP_FILE_ROW)/* "Swap size:  "                     */
#define SWAP_SIZE_COL    35

#define MAX_SWAP_ROW     (SWAP_FILE_ROW)/* "Max swap size:  "                 */
#define MAX_SWAP_COL     (MAX_RGN_COL+2)/* 2 shorter than "Max region size:"  */

#define TEXT_ROW         4
#define TEXT_COL         1

#define DONE_ROW        25
#define DONE_COL         1


/******************************************************************************
*   Procedures global within this file
******************************************************************************/
void    Abbrev_path(char *buffer, char *filename, int max_size);
void    Display_help(void);
void    Display_live_info(void);
int     Display_menu(void);
void    Error(char *msg);
int     Init(void);
void    Main_menu(void);
void    Mod_dos(void);
void    Mod_swap(void);
void    Mod_win(void);
void    Modify_files(unsigned long new_rgn);
int     Open_CB_name(void);
int     Open_filename(void);
unsigned long    Value_K(unsigned long max, unsigned long min, char *desc);
int    Write_region(unsigned long new_rgn);
void    Cleanup(void);
