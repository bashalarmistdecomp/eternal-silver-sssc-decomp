/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp (src/main/psxsdk/libsnd/next.c)
   and psy-q-decomp (src/snd/next.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member next.o: _SsSndNextSep. The first split had it in
   psyq_misc_i.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* libsnd's per-score sequence state (_ss_score[seq][sep], 0xB0 bytes each;
   psyz's and sotn-decomp's struct SeqStruct, at this library version's
   offsets).  The names are docs/spec/code/subsystems/psyq-libs.md's. */
extern SsScore *D_800E9628[];  /* _ss_score */
void _SsSndNextSep(s16, s16);

/* cc97_lib: _SsSndNextSep */

/* libsnd _SsSndNextSep (NEXT.OBJ): start a score from its first event,
   for one repetition.  l_count = 1, l_count_done = 0, the flags lose
   0x100, 8 (replay), 2 (pause), 4 (stop) and 0x200 (finished), unk14 = 1,
   read_pos = unk4 (the first event), and flag 1 (playing) is set.  Its
   caller here is _SsSeqGetEof, for the score chained to one that has
   finished. */
/* func_80099088 */
void _SsSndNextSep(s16 seq_no, s16 sep_no)
{
    SsScore *score;

    score = &D_800E9628[seq_no][sep_no];
    score->l_count = 1;
    score->l_count_done = 0;
    D_800E9628[seq_no][sep_no].flags &= ~0x100;
    D_800E9628[seq_no][sep_no].flags &= ~8;
    D_800E9628[seq_no][sep_no].flags &= ~2;
    D_800E9628[seq_no][sep_no].flags &= ~4;
    D_800E9628[seq_no][sep_no].flags &= ~0x200;
    score->play_mode = 1;
    score->read_pos = score->start_pos;
    D_800E9628[seq_no][sep_no].flags |= 1;
}

INCLUDE_PAD(4);
