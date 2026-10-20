/* PSY-Q 4.5 libetc, member intr_dma.o, part 2 of 2: func_8011FBFC,
   func_8011FD7C, func_8011FE28. The first split had it in main_j.c. The
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
extern u32 var_a0;

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

INCLUDE_ASM("asm/nonmatchings/main", func_8011FBFC);

INCLUDE_ASM("asm/nonmatchings/main", func_8011FD7C);

void func_8011FE28(s32 *arg0, s32 arg1)
{
  int new_var;
  s32 *var_a0;
  s32 var_v0;
  var_a0 = arg0;
  var_v0 = arg1 - 1;
  new_var = 4;
  if (arg1 != 0)
  {
    do
    {
      *var_a0 = 0;
      new_var = 1;
      var_v0 -= new_var;
      var_a0 += new_var;
    }
    while (var_v0 != (-1));
  }
}

INCLUDE_PAD(4);
