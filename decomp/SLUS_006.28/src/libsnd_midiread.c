/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/seqread.c), MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member midiread.o: _SsSeqPlay, _SsSeqGetEof,
   _SsGetSeqData. The first split had it in psyq_misc_i.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* libsnd's per-score sequence state (_ss_score[seq][sep], 0xB0 bytes each;
   psyz's and sotn-decomp's struct SeqStruct, at this library version's
   offsets).  The names are docs/spec/code/subsystems/psyq-libs.md's. */
extern SsScore *D_800E9628[];  /* _ss_score */
extern _SsFCALL D_800E9588;
s32 _SsReadDeltaValue(s16, s16);
s32 _SsGetSeqData(s16, s16);
void _SsSndNextSep(s16, s16);
void _SsVmSeqKeyOff(s16);
/* cc97_lib: _SsSeqPlay */
/* gnu_as_loads: _SsSeqPlay */
/* return_slot_fill: _SsSeqPlay */

/* libsnd _SsSeqPlay (MIDIREAD.OBJ): advance one score (seq, sep) by one
   tick.  While more than tick_step of delta time remains: tick_wait > 0
   counts it down; tick_wait == 0 reloads it from tick_step and takes 1
   off the delta; tick_wait < 0 takes tick_step off the delta.
   Otherwise, when delta <= tick_step: read events with _SsGetSeqData
   (again while it leaves a zero delta), adding each new delta to the old
   one until the sum reaches tick_step, and keep the remainder as the new
   delta. */
/* func_80098998 */
void _SsSeqPlay(s16 seq_no, s16 sep_no)
{
    SsScore *score;
    s32 delta;

    score = &D_800E9628[seq_no][sep_no];
    if (score->delta_value - score->tick_step > 0) {
        if (score->tick_wait > 0) {
            score->tick_wait--;
        } else if (score->tick_wait == 0) {
            score->tick_wait = score->tick_step;
            score->delta_value--;
        } else {
            score->delta_value -= score->tick_step;
        }
    } else if (score->delta_value <= score->tick_step) {
        delta = score->delta_value;
        do {
            do {
                _SsGetSeqData(seq_no, sep_no);
            } while (score->delta_value == 0);
            delta += score->delta_value;
        } while (delta < score->tick_step);
        score->delta_value = delta - score->tick_step;
    }
}
/* cc97_lib: _SsSeqGetEof */
/* return_slot_fill: _SsSeqGetEof */

/* libsnd _SsSeqGetEof (MIDIREAD.OBJ): the end of the track (meta event
   0x2F, or the end point unk10).  Counts one repetition in
   l_count_done.  l_count == 0 (for ever): rewind read_pos, zero unk88,
   loop_armed and delta_value.  Fewer repetitions done than l_count: the
   same, and unk8 takes the rewind position too.  Otherwise the score is
   finished: flags lose 1, 8 and 2 and gain 0x200 and 4, unk14 = 0, unk8
   takes the rewind position, a chained score (next_seq != -1) is
   started with _SsSndNextSep, this score's voices are keyed off with
   _SsVmSeqKeyOff (twice when a score was chained), and delta_value =
   tick_step.  The rewind position is unkC when flag 0x400 is set, unk4
   otherwise.  The third argument is never read. */
/* func_80098A94 */
void _SsSeqGetEof(s16 seq_no, s16 sep_no, u8 type)
{
    SsScore *score;

    score = &D_800E9628[seq_no][sep_no];
    score->l_count_done++;
    if (score->l_count == 0) {
        score->delta_sum = 0;
        score->loop_armed = 0;
        score->delta_value = 0;
        if (D_800E9628[seq_no][sep_no].flags & 0x400) {
            score->read_pos = score->rewind_pos;
        } else {
            score->read_pos = score->start_pos;
        }
    } else if (score->l_count_done < score->l_count) {
        score->delta_sum = 0;
        score->loop_armed = 0;
        score->delta_value = 0;
        if (D_800E9628[seq_no][sep_no].flags & 0x400) {
            score->read_pos = score->rewind_pos;
            score->loop_pos = score->rewind_pos;
        } else {
            score->read_pos = score->start_pos;
            score->loop_pos = score->start_pos;
        }
    } else {
        D_800E9628[seq_no][sep_no].flags &= ~1;
        D_800E9628[seq_no][sep_no].flags &= ~8;
        D_800E9628[seq_no][sep_no].flags &= ~2;
        D_800E9628[seq_no][sep_no].flags |= 0x200;
        D_800E9628[seq_no][sep_no].flags |= 4;
        score->play_mode = 0;
        if (D_800E9628[seq_no][sep_no].flags & 0x400) {
            score->loop_pos = score->rewind_pos;
        } else {
            score->loop_pos = score->start_pos;
        }
        if (score->next_seq != -1) {
            score->play_mode = 0;
            _SsSndNextSep(score->next_seq, score->next_sep);
            _SsVmSeqKeyOff(seq_no | (sep_no << 8));
        }
        _SsVmSeqKeyOff(seq_no | (sep_no << 8));
        score->delta_value = score->tick_step;
    }
}
/* cc97_lib: _SsGetSeqData */
/* return_slot_fill: _SsGetSeqData */

/* libsnd _SsGetSeqData (MIDIREAD.OBJ): read one SEQ event of a score and
   hand it to its handler.  Takes the next byte at read_pos.  While flags
   0x401 are both set and that byte was the one at the end point unk10,
   the track ends there: _SsSeqGetEof, return -1.  A byte with bit 7 set
   is a status byte: its low nibble goes to channel, its high nibble is
   kept as the running status in unk16 (0xFF for 0xF0), and the data
   bytes follow; any other byte is the first data byte of an event under
   the running status.  0x90: note and velocity, then delta_value =
   _SsReadDeltaValue and SsFCALL.noteon(seq, sep, note, vel).  0xB0:
   SsFCALL.control[CC_NUMBER](seq, sep, byte).  0xC0:
   SsFCALL.programchange(seq, sep, byte).  0xE0: skips one byte (only
   after a status byte) and calls SsFCALL.pitchbend(seq, sep).  0xF0 /
   0xFF: a meta event; type 0x2F is the end of the track (_SsSeqGetEof,
   return 1), any other type goes to SsFCALL.metaevent(seq, sep, type).
   Returns 0 otherwise, also for a status it does not know. */
/* func_80098CD8 */
s32 _SsGetSeqData(s16 seq_no, s16 sep_no)
{
    SsScore *score;
    u8 data;
    u8 note;
    u8 vel;
    u8 arg;
    s32 ret;

    score = &D_800E9628[seq_no][sep_no];
    data = *score->read_pos++;
    ret = 0;
    if ((D_800E9628[seq_no][sep_no].flags & 0x401) == 0x401) {
        if (score->read_pos == score->end_pos + 1) {
            _SsSeqGetEof(seq_no, sep_no, score->end_pos[1]);
            return -1;
        }
    }
    if (data & 0x80) {
        score->channel = data & 0x0F;
        switch (data & 0xF0) {
        case 0x90:
            score->running_status = 0x90;
            note = *score->read_pos++;
            vel = *score->read_pos++;
            score->delta_value = _SsReadDeltaValue(seq_no, sep_no);
            D_800E9588.noteon(seq_no, sep_no, note, vel);
            break;
        case 0xB0:
            score->running_status = 0xB0;
            arg = *score->read_pos++;
            D_800E9588.control[0](seq_no, sep_no, arg);
            break;
        case 0xC0:
            score->running_status = 0xC0;
            arg = *score->read_pos++;
            D_800E9588.programchange(seq_no, sep_no, arg);
            break;
        case 0xE0:
            score->running_status = 0xE0;
            score->read_pos++;
            D_800E9588.pitchbend(seq_no, sep_no);
            break;
        case 0xF0:
            score->running_status = 0xFF;
            arg = *score->read_pos++;
            if (arg == 0x2F) {
                ret = 1;
                _SsSeqGetEof(seq_no, sep_no, arg);
            } else {
                D_800E9588.metaevent(seq_no, sep_no, arg);
            }
            break;
        }
    } else {
        switch (score->running_status) {
        case 0x90:
            vel = *score->read_pos++;
            score->delta_value = _SsReadDeltaValue(seq_no, sep_no);
            D_800E9588.noteon(seq_no, sep_no, data, vel);
            break;
        case 0xB0:
            D_800E9588.control[0](seq_no, sep_no, data);
            break;
        case 0xC0:
            D_800E9588.programchange(seq_no, sep_no, data);
            break;
        case 0xE0:
            D_800E9588.pitchbend(seq_no, sep_no);
            break;
        case 0xFF:
            if (data == 0x2F) {
                ret = 1;
                _SsSeqGetEof(seq_no, sep_no, data);
            } else {
                D_800E9588.metaevent(seq_no, sep_no, data);
            }
            break;
        }
    }
    return ret;
}

INCLUDE_PAD(4);
