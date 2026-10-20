/* PSY-Q 4.5 libcd, member c_011.o, part 2 of 2: func_8012902C,
   func_80129058. The first split had it in main_ag.c. The tree compiles
   this run of the member's functions with other flags than the runs beside
   it (config/cc_flags.txt), so the member is in several files.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern volatile int arg0;
extern s32 arg1;
extern s32 arg2;
extern unsigned short new_var;
extern u32 var_a0;
extern u8 *var_a1;
extern unsigned int var_v1;

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

void func_8012902C(s32 *arg0, s32 *arg1, u32 arg2)
{
  s32 *var_a0;
  int new_var2;
  s32 *var_a1;
  int new_var;
  s32 temp_v0;
  u32 var_v1;
  new_var = (new_var = 4);
  var_a0 = arg0;
  var_a1 = arg1;
  var_v1 = 0;
  if (arg2 != 0)
  {
    do
    {
      new_var2 = 1;
      new_var = new_var2;
      temp_v0 = *var_a1;
      var_a1 += new_var;
      var_v1 += new_var;
      *var_a0 = temp_v0;
      var_a0 += new_var;
    }
    while (var_v1 < arg2);
  }
}

INCLUDE_ASM("asm/nonmatchings/main", func_80129058);
