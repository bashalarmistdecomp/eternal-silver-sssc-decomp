/* PSY-Q 4.5 libpress, member libpress.o, part 1 of 4: DecDCTReset,
   DecDCTGetEnv, DecDCTPutEnv, DecDCTin, DecDCTout, DecDCTinSync,
   DecDCToutSync, DecDCTinCallback, DecDCToutCallback. The first split had
   it in main_ag.c. The tree compiles this run of the member's functions
   with other flags than the runs beside it (config/cc_flags.txt), so the
   member is in several files.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern volatile int arg0;
extern s32 arg1;
extern unsigned short new_var;
extern u8 *new_var3;

/* redistributed from the original TU (split decl repair) */
s32 ResetCallback();
s32 DMACallback(s32, s32);
void DecDCTReset(s32 arg0);  /* TU defines this below */
void DecDCTout(s32, s32);  /* TU defines this below */
s32 DecDCToutCallback(s32 arg0);  /* TU defines this below */
s32 MDEC_rest(s32);
extern void (*D_801336C8_s)();

/* func_80129390 */
void DecDCTReset(s32 arg0) {
    if (arg0 == 0) {
        ResetCallback();
    }
    MDEC_rest(arg0);
}

/* func_801293C4 */
INCLUDE_ASM("asm/nonmatchings/main", DecDCTGetEnv);

/* func_80129450 */
INCLUDE_ASM("asm/nonmatchings/main", DecDCTPutEnv);

/* func_801294E8 */
void DecDCTin(s32 *arg0, s32 arg1)
{
  s32 new_var3;
  s32 new_var2;
  s32 var_v0;
  int new_var;
  s32 var_v0_2;
  new_var3 = (var_v0_2 = 0x02000000);
  new_var2 = 1;
  if (arg1 & new_var2)
  {
    new_var = 0xF7FFFFFF;
    var_v0 = (*arg0 = (*arg0) & new_var);
  }
  else
  {
    var_v0 = (*arg0) | 0x08000000;
    *arg0 = var_v0;
  }
  new_var2 = new_var3;
  if (arg1 & 2)
  {
    var_v0_2 = (*arg0) | new_var2;
  }
  else
  {
    var_v0_2 = (*arg0) & 0xFDFFFFFF;
  }
  *arg0 = var_v0_2;
  func_80129734((s32) arg0, (u16) (*arg0));
}

/* epilogue_load_slot: DecDCTout */
/* func_80129564 */
void DecDCTout(s32 arg0, s32 arg1) {
    func_801297C4(arg0, arg1);
}

s32 MDEC_in_sync();
u32 func_80129978();

/* func_80129584 */
s32 DecDCTinSync(s32 arg0)
{
  if (arg0 == 0)
  {
    return MDEC_in_sync();
  }
  if (arg0)
  {
    return (func_80129978() >> 0x1D) & 1;
  }
  else
  {
    return (func_80129978() >> 0x1D) & 1;
  }
}

s32 MDEC_out_sync();

/* func_801295C0 */
s32 DecDCToutSync(s32 arg0)
{
  if (arg0 == 0)
  {
    if (arg0)
    {
      return MDEC_out_sync();
    }
    else
    {
      return MDEC_out_sync();
    }
  }
  return (func_80129978() >> 0x18) & 1;
}

/* func_801295FC */
void DecDCTinCallback(s32 arg0)
{
  unsigned long long new_var;
  if (new_var)
  {
    new_var = 0;
    DMACallback(new_var, arg0);
  }
  else
  {
    new_var = 0;
    DMACallback(new_var, arg0);
  }
}

/* func_80129620 */
s32 DecDCToutCallback(s32 arg0)
{
  int new_var2;
  unsigned int new_var;
  if (new_var2)
  {
    new_var2 = 1;
    new_var = new_var2;
    return DMACallback(new_var, arg0);
  }
  else
  {
    new_var2 = 1;
    new_var = new_var2;
    return DMACallback(new_var, arg0);
  }
}
