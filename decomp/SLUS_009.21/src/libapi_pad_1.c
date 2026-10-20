/* SPDX-License-Identifier: MIT
   Parts of this file match code of psyz (decomp/src/libapi/pad.c),
   MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libapi, member pad.o, part 1 of 4: ReadInitPadFlag, PAD_init,
   InitPAD, StartPAD. The first split had it in main_j.c. The tree compiles
   this run of the member's functions with other flags than the runs beside
   it (config/cc_flags.txt), so the member is in several files.
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
extern s32 D_80132ADC;  /* far_syms */
void PAD_init(s32 arg0, s32 arg1, s32 arg2, s32 arg3);  /* TU defines this below */
s32 StartPAD2();
extern void (*D_801336C8_s)();

/* func_8011FF0C */
s32 ReadInitPadFlag(void) {
    return D_80132ADC;
}

/* func_8011FF1C */
void PAD_init(s32 arg0, s32 arg1, s32 arg2, s32 arg3)
{
  _remove_ChgclrPAD();
  EnterCriticalSection();
 do { _patch_pad(); ExitCriticalSection(); ChangeClearPAD(0); func_801200A4(); PAD_init2(arg0, arg1, arg2, arg3); D_80132ADC = 1; } while (0);
}

/* func_8011FFAC */
long InitPAD(char *arg0, long arg1, char *arg2, long arg3)
{
  s32 new_var;
  int new_var2;
  _remove_ChgclrPAD();
  new_var = arg0;
  EnterCriticalSection();
  _patch_pad();
  ExitCriticalSection();
  ChangeClearPAD(0);
  func_801200A4();
  new_var2 = 1;
  InitPAD2(new_var, arg1, arg2, arg3);
 do { D_80132ADC = new_var2; } while (0);
}

/* func_8012003C */
long StartPAD(void) {
    StartPAD2();
    ChangeClearPAD(0);
    EnablePAD();
    return 1;
}
