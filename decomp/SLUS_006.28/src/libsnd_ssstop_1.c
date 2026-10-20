/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/stop.c), psyz (decomp/src/libsnd/ssstop.c) and
   psy-q-decomp (src/snd/stop.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member ssstop.o, part 1 of 2: _SsSndStop. The first
   split had it in psyq_misc_i.c. The tree compiles this run of the member's
   functions with other flags than the runs beside it (config/cc_flags.txt),
   so the member is in several files.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* libsnd's per-score sequence state (_ss_score[seq][sep], 0xB0 bytes each;
   psyz's and sotn-decomp's struct SeqStruct, at this library version's
   offsets).  The names are docs/spec/code/subsystems/psyq-libs.md's. */
extern SsScore *D_800E9628[];  /* _ss_score */
void _SsVmDamperOff(void);
void _SsVmSeqKeyOff(s16);
/* cc97_lib: _SsSndStop */
/* return_slot_fill: _SsSndStop */

/* libsnd _SsSndStop (SSSTOP.OBJ): stop a score and rewind it.  The flags
   lose 1 (playing), 2 (pause), 8 (replay) and 0x400 and gain 4 (stop
   requested); _SsVmSeqKeyOff(seq | sep << 8) keys off its voices and
   _SsVmDamperOff drops the damper.  Then the reader's
   state goes back to what _SsInitSoundSeq left: unk14, unk88, the RPN /
   NRPN bytes and counters, channel, l_count_done, the loop bytes and the
   running status = 0; delta_value = unk84 (the first event's delta time),
   tempo = hdr_tempo, tick_step = unk56, read_pos = unk8 = unk4 (the first
   event); every channel's program = its number, pan = 64, volume = 127;
   cres_voll = cres_volr = 127.  SsSeqCalledTbyT calls it for a score
   whose flag 4 is set; SsSeqStop / SsSepStop (libsnd_ssstop_2.c) call it
   directly. */
/* func_800991E8 */
void _SsSndStop(s16 seq_no, s16 sep_no)
{
    SsScore *score;
    s32 i;

    score = &D_800E9628[seq_no][sep_no];
    D_800E9628[seq_no][sep_no].flags &= ~1;
    D_800E9628[seq_no][sep_no].flags &= ~2;
    D_800E9628[seq_no][sep_no].flags &= ~8;
    D_800E9628[seq_no][sep_no].flags &= ~0x400;
    D_800E9628[seq_no][sep_no].flags |= 4;
    _SsVmSeqKeyOff(seq_no | (sep_no << 8));
    _SsVmDamperOff();
    score->play_mode = 0;
    score->delta_sum = 0;
    score->loop_armed = 0;
    score->rpn_lsb = 0;
    score->rpn_msb = 0;
    score->rpn_count = 0;
    score->nrpn_lsb = 0;
    score->nrpn_msb = 0;
    score->nrpn_count = 0;
    score->channel = 0;
    score->l_count_done = 0;
    score->loop_armed = 0;
    score->loop_count = 0;
    score->loop_on = 0;
    score->running_status = 0;
    score->delta_value = score->start_delta;
    AS(s32, score->tempo) = score->hdr_tempo;
    score->tick_step = score->hdr_tick_step;
    score->read_pos = score->start_pos;
    score->loop_pos = score->start_pos;
    for (i = 0; i < 16; i++) {
        score->programs[i] = i;
        score->panpot[i] = 64;
        score->vol[i] = 127;
    }
    score->cres_voll = 127;
    score->cres_volr = 127;
}
