/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/ssclose.c), psyz (decomp/src/libsnd/ssclose.c) and
   psy-q-decomp (src/snd/ssclose.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member ssclose.o, part 1 of 2: func_80095758. The first
   split had it in psyq_spu_k.c. The tree compiles this run of the member's
   functions with other flags than the runs beside it (config/cc_flags.txt),
   so the member is in several files.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: func_800954D4 */
/* return_slot_fill: func_800954D4 */

/* cc97_lib: func_80095758 */
/* return_slot_fill: func_80095758 */
extern SsScore *D_800E9628[];           /* _ss_score */
extern u32 D_800E9620;                  /* _snd_openflag: bit a = access number a is open */
extern s16 D_800E9EAA;                  /* _snd_seq_t_max: records per row of _ss_score */
s16 _SsVmSetSeqVol(s16, u16, u16, s16);
void _SsVmSeqKeyOff(s16);

/* libsnd _SsClose (SSCLOSE.OBJ, static): the worker of SsSeqClose
    and SsSepClose.  Silences access number
   `seq_sep_num` (_SsVmSetSeqVol(n, 0, 0, 1), then _SsVmSeqKeyOff(n)), clears
   its bit in _snd_openflag so the tick no longer serves it and SsSeqOpen can
   hand the number out again, and resets every record of its row
   (_snd_seq_t_max of them): the play flags, the chained next score (-1 =
   none), the crescendo and tempo-ramp state, and the volume pair back to
   127 / 127. */
void func_80095758(s16 seq_sep_num)
{
    s32 seq_num;

    _SsVmSetSeqVol(seq_sep_num, 0, 0, 1);
    _SsVmSeqKeyOff(seq_sep_num);
    D_800E9620 &= ~(1 << seq_sep_num);
    for (seq_num = 0; seq_num < D_800E9EAA; seq_num++) {
        D_800E9628[seq_sep_num][seq_num].flags = 0;
        D_800E9628[seq_sep_num][seq_num].next_seq = -1;
        D_800E9628[seq_sep_num][seq_num].next_sep = 0;
        D_800E9628[seq_sep_num][seq_num].cres_vol = 0;
        AS(u16, D_800E9628[seq_sep_num][seq_num].cres_done) = 0;
        D_800E9628[seq_sep_num][seq_num].cres_time = 0;
        D_800E9628[seq_sep_num][seq_num].cres_tick = 0;
        D_800E9628[seq_sep_num][seq_num].unk_4c = 0;
        D_800E9628[seq_sep_num][seq_num].tempo_goal = 0;
        D_800E9628[seq_sep_num][seq_num].tempo_count = 0;
        D_800E9628[seq_sep_num][seq_num].unk_a4 = 0;
        D_800E9628[seq_sep_num][seq_num].tempo_step = 0;
        D_800E9628[seq_sep_num][seq_num].voll = 127;
        D_800E9628[seq_sep_num][seq_num].volr = 127;
    }
}
