/* PSY-Q 4.5 libpad, member padcmd.o, part 5 of 7: _padSetActAlign,
   func_8012E094, func_8012E0B0, _padSetMainMode, func_8012E210. The first
   split had it in main_aae.c. The tree compiles this run of the member's
   functions with other flags than the runs beside it (config/cc_flags.txt),
   so the member is in several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern volatile int arg0;
extern unsigned short new_var;
extern int new_var2;

/* redistributed from the original TU (split decl repair) */
void func_8012E094(void *arg0);  /* TU defines this below */
extern void (*D_801336C8_s)();

/* func_8012E02C */
INCLUDE_ASM("asm/nonmatchings/main", _padSetActAlign);

void func_8012E094(void *arg0)
{
  s32 *new_var2;
  void *new_var;
  new_var = arg0;
  *((u8 *) (((u8 *) new_var) + 0x37)) = 0x4D;
  *(((u8 *) arg0) + 0x36) = 6;
  new_var2 = (s32 *) (((u8 *) arg0) + 0x20);
  *((s32 *) (((u8 *) new_var) + 0x2c)) = (s32) (*new_var2);
}

INCLUDE_ASM("asm/nonmatchings/main", func_8012E0B0);

/* func_8012E178 */
INCLUDE_ASM("asm/nonmatchings/main", _padSetMainMode);

INCLUDE_ASM("asm/nonmatchings/main", func_8012E210);
