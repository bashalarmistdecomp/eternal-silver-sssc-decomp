/* PSY-Q 4.5 libpad, member padcmd.o, part 3 of 7: _padRecvAtLoadInfo,
   _padGetActSize, _padLoadActInfo. The first split had it in main_aac.c.
   The tree compiles this run of the member's functions with other flags
   than the runs beside it (config/cc_flags.txt), so the member is in
   several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern volatile int arg0;
extern unsigned short new_var;
extern int new_var2;

/* redistributed from the original TU (split decl repair) */
s32 _padGetActSize(void *arg0);  /* TU defines this below */
extern void (*D_801336C8_s)();

/* func_8012D810 */
INCLUDE_ASM("asm/nonmatchings/main", _padRecvAtLoadInfo);

/* func_8012DA80 */
s32 _padGetActSize(void *arg0)
{
  u8 new_var2;
  int new_var;
  new_var2 = *((u8 *) (((u8 *) arg0) - (-0xe9)));
  return (((((s32) ((*((u8 *) (((u8 *) arg0) + 0xe3))) + 1)) >> 1) * (new_var = 4)) + ((((new_var2 * 5) + 3) & 0xFFC) + new_var)) + ((unsigned short) (*((s32 *) (((u8 *) arg0) + 0xec))));
}

/* func_8012DAB8 */
INCLUDE_ASM("asm/nonmatchings/main", _padLoadActInfo);
