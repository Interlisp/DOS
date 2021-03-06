/******************************************************************************
* Intel 386/486 C Code Builder(TM) Kit
* Copyright 1991 Intel Corporation.  All Rights Reserved.
*******************************************************************************
* MOUSETST.H
*
* Mouse test definitions file.
******************************************************************************/

/******************************************************************************
* Mouse event flag defines
******************************************************************************/
#define MOUSE_MV  0x01                            /* Mouse movement occurred */
#define LB_PRESS  0x02                            /* Left button pressed     */
#define LB_OFF    0x04                            /* Left button released    */
#define RB_PRESS  0x08                            /* Right button pressed    */
#define RB_OFF    0x10                            /* Right button released   */
#define CB_OFF    0x20                            /* Center button released  */
#define CB_PRESS  0x40                            /* Center button pressed   */

/******************************************************************************
* This documents the contents of the mouse registers as passed to the
* user's mouse handler.  The values of these registers are contained in the
* _XSTACK frame as with all interrupt routines.  A pointer to the _XSTACK
* frame can be retrieved by using '_get_stk_frame()' at the start of an
* interrupt routine.  See the include file 'STK.H' for the definition of
* the _XSTACK structure.
*
* The registers are set, as passed from the mouse driver, as follows:
*
*       ax - Mouse event flags, as follows:
*             Bit   Meaning if set
*             ===   ==============
*             0     Mouse movement
*             1     Left button pressed
*             2     Left button released
*             3     Right button pressed
*             4     Right button released
*             5     Center button pressed
*             6     Center button released
*
*       bx - Button state, as follows:
*             Bit   Meaning if set
*             ===   ==============
*             0     Left button is down
*             1     Right button is down
*             2     Center button is down
*
*       cx - Horizontal (X) mouse pointer coordinate.
*
*       dx - Vertical (Y) mouse pointer coordinate.
*
*       si - Last raw vertical mickey count.
*
*       di - Last raw horizontal mickey count.
******************************************************************************/

