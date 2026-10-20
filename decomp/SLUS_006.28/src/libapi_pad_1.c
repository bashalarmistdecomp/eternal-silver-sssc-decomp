/* SPDX-License-Identifier: MIT
   Parts of this file match code of psyz (decomp/src/libapi/pad.c),
   MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libapi, member pad.o, part 1 of 4: SetInitPadFlag,
   ReadInitPadFlag, PAD_init, InitPAD, StartPAD. The first split had it in
   psyq_misc_al.c. The tree compiles this run of the member's functions with
   other flags than the runs beside it (config/cc_flags.txt), so the member
   is in several files.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
extern s32 D_800B8440;

s32 EnterCriticalSection();                                /* extern */
s32 ReadInitPadFlag();                                
/* cc97_lib: SetInitPadFlag */
/* return_slot_fill: SetInitPadFlag */

/* libapi SetInitPadFlag (PAD.OBJ): store the pad-initialised flag (D_800B8440, PAD.OBJ
   .data+0) that ReadInitPadFlag returns */
/* func_800A11B8 */
void SetInitPadFlag(s32 flag)
{
    D_800B8440 = flag;
}

/* cc97_lib: ReadInitPadFlag */
/* far_syms: D_800B8440 (target reaches it %hi/%lo) */

/* libapi ReadInitPadFlag (PAD.OBJ): the pad-initialised flag (D_800B8440) that PAD_init
   and InitPAD set, StopPAD clears and SetInitPadFlag stores. */
/* func_800A11C4 */
s32 ReadInitPadFlag(void) {
    return D_800B8440;
}
s32 func_800A135C();
s32 PAD_init2(s32, s32, s32, s32);
s32 _patch_pad();
s32 _remove_ChgclrPAD();

/* cc97_lib: PAD_init */
/* return_slot_fill: PAD_init */

/* libapi PAD_init (PAD.OBJ): _remove_ChgclrPAD, _patch_pad inside a critical section,
   ChangeClearPAD(0), SetPatchPad, the BIOS PAD_init2 with the four
   arguments, then the pad-initialised flag D_800B8440 = 1 */
/* func_800A11D4 */
void PAD_init(s32 arg0, s32 arg1, s32 arg2, s32 arg3)
{
    _remove_ChgclrPAD();
    EnterCriticalSection();
    _patch_pad();
    ExitCriticalSection();
    ChangeClearPAD(0);
    func_800A135C();
    PAD_init2(arg0, arg1, arg2, arg3);
    D_800B8440 = 1;
}
s32 InitPAD2(s32, s32, s32, s32);

/* cc97_lib: InitPAD */
/* return_slot_fill: InitPAD */

/* libapi InitPAD (PAD.OBJ): PAD_init's twin, with the BIOS InitPAD2 in
   place of PAD_init2 */
/* func_800A1264 */
long InitPAD(char *arg0, long arg1, char *arg2, long arg3)
{
    _remove_ChgclrPAD();
    EnterCriticalSection();
    _patch_pad();
    ExitCriticalSection();
    ChangeClearPAD(0);
    func_800A135C();
    InitPAD2(arg0, arg1, arg2, arg3);
    D_800B8440 = 1;
}

/* cc97_lib: StartPAD */
s32 StartPAD2();                                  /* extern */

/* libapi StartPAD (PAD.OBJ): the BIOS StartPAD2, ChangeClearPAD(0), then EnablePAD.
   Returns 1. */
/* func_800A12F4 */
long StartPAD(void) {
    StartPAD2();
    ChangeClearPAD(0);
    EnablePAD();
    return 1;
}
