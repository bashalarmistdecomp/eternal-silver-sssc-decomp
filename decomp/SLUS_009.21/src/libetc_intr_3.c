/* PSY-Q 4.5 libetc, member intr.o, part 3 of 3: SetIntrMask, func_8011F564,
   func_8011F63C, func_8011F80C, func_8011F954, func_8011F9F4,
   func_8011FA6C. The first split had it in main_f.c. The tree compiles this
   run of the member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern volatile int arg0;
extern s32 arg1;
extern u32 var_a0;
extern void (*D_801336C8_s)();

/* func_8011F54C */
INCLUDE_ASM("asm/nonmatchings/main", SetIntrMask);

INCLUDE_ASM("asm/nonmatchings/main", func_8011F564);

INCLUDE_ASM("asm/nonmatchings/main", func_8011F63C);

INCLUDE_ASM("asm/nonmatchings/main", func_8011F80C);

INCLUDE_ASM("asm/nonmatchings/main", func_8011F954);

INCLUDE_ASM("asm/nonmatchings/main", func_8011F9F4);

void func_8011FA6C(s32 *arg0, s32 arg1)
{
  s32 *var_a0;
  s32 var_v0;
  unsigned short new_var;
  var_a0 = arg0;
  var_v0 = arg1 - 1;
  new_var = 4;
  if (arg1 != 0)
  {
    do
    {
      *var_a0 = 0;
      var_v0 -= (new_var = 1);
      var_a0 += new_var;
    }
    while (var_v0 != (-1));
  }
}
