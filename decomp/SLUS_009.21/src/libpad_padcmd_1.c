/* PSY-Q 4.5 libpad, member padcmd.o, part 1 of 7: _padSetAct, _padSetCmd.
   The first split had it in main_aaa.c. The tree compiles this run of the
   member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern volatile int arg0;
extern s32 arg1;
extern s32 arg2;
extern s32 arg3;
extern unsigned short new_var;

/* redistributed from the original TU (split decl repair) */
void _padSetCmd(void *arg0, s8 arg1, s32 arg2, s8 arg3);  /* TU defines this below */
extern void (*D_801336C8_s)();

/* func_8012D770 */
void _padSetAct(void *arg0, s32 arg1, s8 arg2)
{
  *((s32 *) (((u8 *) arg0) + 0x28)) = arg1;
  *(((u8 *) arg0) + 0x34) = arg2;
}

/* func_8012D77C */
void _padSetCmd(void *arg0, s8 arg1, s32 arg2, s8 arg3)
{
  u8 *new_var;
  *((u8 *) (((u8 *) arg0) + 0x37)) = arg1;
  *((s32 *) (((u8 *) arg0) + 0x2c)) = arg2;
  new_var = (u8 *) arg0;
  *(new_var + 0x36) = arg3;
}
