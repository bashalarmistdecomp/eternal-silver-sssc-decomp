/* PSY-Q 4.5 libetc, member intr_vb.o, part 2 of 2: func_8011FAE8,
   setIntrVSync, func_8011FB80. The first split had it in main_h.c. The
   tree compiles this run of the member's functions with other flags than
   the runs beside it (config/cc_flags.txt), so the member is in several
   files.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern volatile int arg0;
extern s32 arg1;
extern unsigned short new_var;
extern s32 temp_a0;

/* redistributed from the original TU (split decl repair) */
extern u8 D_80132A7C[];
void setIntrVSync(s32 arg0, s32 arg1);  /* TU defines this below */
extern void (*D_801336C8_s)();

INCLUDE_ASM("asm/nonmatchings/main", func_8011FAE8);

/* func_8011FB54 */
void setIntrVSync(s32 arg0, s32 arg1)
{
  u8 *new_var;
  s32 *temp_a0;
 new_var = D_80132A7C; do { temp_a0 = (arg0 * 4) + new_var; if (arg1 != (*temp_a0)) { *temp_a0 = arg1; } } while (0);
}

INCLUDE_ASM("asm/nonmatchings/main", func_8011FB80);
