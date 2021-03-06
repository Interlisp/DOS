#include <i32.h>                             /* Defines "#pragma interrupt"  */
#include <dos.h>                             /* Defines REGS & other structs */
#include <stk.h>                             /* _XSTACK struct definition    */

#pragma interrupt(Mouse)

int foo(a)
     int a;
{
  return(a++);
}

void  Mouse(void)
{
  _XSTACK  *stk_ptr;

  /* First save the stack frame. */
  stk_ptr = (_XSTACK *)_get_stk_frame(); /* Get ptr to V86 _XSTACK frame */
}

main()
{
  foo(666);
}
