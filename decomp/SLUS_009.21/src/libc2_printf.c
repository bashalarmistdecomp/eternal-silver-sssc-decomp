/* PSY-Q 4.5 libc2, member printf.o: printf. The first split had it in
   main_aai.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern unsigned short new_var;

/* redistributed from the original TU (split decl repair) */
void printf();  /* TU defines this below */
extern void (*D_801336C8_s)();

/* printf-family: spills arg1..3 beside each other and hands &arg1 to the
   vsprintf core (prnt). Callers pass 1..8 args per site, so the
   definition is K&R — unprototyped, exactly like the original — and m2c
   infers each call site's arity instead of forcing four args. */
/* func_8012FFC0 */
void printf(arg0, arg1, arg2, arg3)
    volatile int arg0;
    s32 arg1;
    s32 arg2;
    s32 arg3;
{
  s32 *new_var;
  *((s32 *) (((u8 *) (&arg1)) + 0x4)) = arg2;
  new_var = &arg1;
  *((s32 *) (((u8 *) new_var) + 0x8)) = arg3;
  prnt(1, arg0, &arg1);
}

INCLUDE_PAD(4);
