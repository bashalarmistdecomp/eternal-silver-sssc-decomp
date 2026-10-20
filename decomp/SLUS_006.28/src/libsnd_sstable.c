/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/sstable.c), psyz (decomp/src/libsnd/sstable.c) and
   psy-q-decomp (src/snd/sstable.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member sstable.o: SsSetTableSize. The first split had
   it in psyq_misc_j.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: SsSeqStop */

/* cc97_lib: SsSetTableSize */
extern s16 D_800E9EA8;              /* _snd_seq_s_max */
extern s16 D_800E9EAA;              /* _snd_seq_t_max */
extern u32 D_800E9620;              /* _snd_openflag */
extern SsScore *D_800E9628[];       /* _ss_score */

/* libsnd SsSetTableSize (SSTABLE.OBJ): hand the library its score table.
   `table` is s_max * t_max records of 0xB0 bytes; _ss_score[i] is set to
   row i (t_max records each).  Access numbers s_max..31 are marked taken
   in _snd_openflag so SsSeqOpen never hands them out.  Then every record
   is reset: not playing, no follow-on score (-1), no crescendo and no
   tempo ramp, volumes 127. */
/* func_800993C8 */
void SsSetTableSize(char *table, short s_max, short t_max)
{
    int i;
    int j;

    D_800E9EA8 = s_max;
    D_800E9EAA = t_max;
    for (i = 0; i < s_max; i++) {
        D_800E9628[i] = (SsScore *)table + i * t_max;
    }
    for (i = s_max; i < 32; i++) {
        D_800E9620 |= 1 << i;
    }
    for (i = 0; i < D_800E9EA8; i++) {
        for (j = 0; j < D_800E9EAA; j++) {
            D_800E9628[i][j].flags = 0;
            D_800E9628[i][j].next_seq = -1;
            D_800E9628[i][j].next_sep = 0;
            D_800E9628[i][j].cres_vol = 0;
            D_800E9628[i][j].cres_done = 0;
            D_800E9628[i][j].cres_time = 0;
            D_800E9628[i][j].cres_tick = 0;
            D_800E9628[i][j].unk_4c = 0;
            D_800E9628[i][j].tempo_goal = 0;
            D_800E9628[i][j].tempo_count = 0;
            D_800E9628[i][j].unk_a4 = 0;
            D_800E9628[i][j].tempo_step = 0;
            D_800E9628[i][j].voll = 127;
            D_800E9628[i][j].volr = 127;
            D_800E9628[i][j].cres_voll = 127;
            D_800E9628[i][j].cres_volr = 127;
        }
    }
}
INCLUDE_PAD(8);
