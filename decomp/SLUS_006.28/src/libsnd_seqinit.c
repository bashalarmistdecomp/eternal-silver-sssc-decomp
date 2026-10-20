/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/seqinit.c) and psyz (decomp/src/libsnd/seqinit.c),
   all MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member seqinit.o: _SsInitSoundSeq. The first split had
   it in psyq_math_e.c.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: _SsSetProgramChange */
/* gnu_as_loads: _SsSetProgramChange */
/* return_slot_fill: _SsSetProgramChange */
/* libsnd's per-score sequence state (_ss_score[seq][sep], 0xB0 bytes each;
   psyz's and sotn-decomp's struct SeqStruct, at this library version's
   offsets).  The names are docs/spec/code/subsystems/psyq-libs.md's. */
extern SsScore *D_800E9628[];  /* _ss_score */
s32 _SsReadDeltaValue(s16, s16);

/* cc97_lib: _SsInitSoundSeq */
/* gnu_as_loads: _SsInitSoundSeq */
/* return_slot_fill: _SsInitSoundSeq */
/* reorder_fill: _SsInitSoundSeq */
extern u32 D_800E9624;
extern char D_80013874[];
extern char D_8001388C[];
s32 printf();

/* libsnd _SsInitSoundSeq(seq_no, vab_id, addr) (seqinit.o; instruction-identical to the
   4.6 and 4.7 library objects): SsSeqOpen's (SsSeqOpen) worker. Resets the first
   record of score table entry seq_no (score = _ss_score[seq_no], D_800E9628, 0xB0-byte
   records) and parses the SEQ header at addr: vab_id, zeroed state, per-channel program =
   channel, pan 0x40, volume 0x7F. The data must start with 'S' or 'p' ("pQES"; otherwise
   printf "This is an old SEQ Data Format." and return 0) and carry
   version byte 7 == 1 (otherwise "This is not SEQ Data." and return -1). Then resolution =
   bytes 8-9 (big-endian), hdr_tempo = 60000000 / bytes 10-12 (microseconds per quarter
   note) rounded to nearest, copied to tempo; the rhythm bytes 13-14; delta_value = +0x84 =
   _SsReadDeltaValue, and the read pointer, now at the first event, saved
   in +0x04/+0x08/+0x0C. The play step is derived as in _SsGetMetaEvent
   : with n = resolution * hdr_tempo and VBLANK_MINUS (D_800E9624) the
   sound service's ticks per second, n * 10 < VBLANK_MINUS * 60 gives tick_wait =
   tick_step = VBLANK_MINUS * 600 / n, otherwise tick_wait = -1 and tick_step = n * 10 /
   (VBLANK_MINUS * 60) rounded to nearest; +0x56 keeps a copy. Returns 0. The recorded
   size ends in three pad nops, hence INCLUDE_PAD(12). */
/* func_80097C48 */
s16 _SsInitSoundSeq(s16 arg0, s16 vab_id, u8 *addr)
{
  SsScore *score;
  s32 channel;
  s32 hi;
  s32 lo;
  s32 a;
  s32 b;
  s32 c;
  score = D_800E9628[arg0];
  score->vab_id = vab_id;
  score->resolution = 0;
  score->rpn_lsb = 0;
  score->rpn_msb = 0;
  score->rpn_count = 0;
  score->nrpn_lsb = 0;
  score->nrpn_msb = 0;
  score->nrpn_count = 0;
  score->channel = 0;
  score->start_delta = 0;
  score->delta_sum = 0;
  score->hdr_tempo = 0;
  score->hdr_tick_step = 0;
  score->l_count_done = 0;
  score->l_count = 1;
  score->play_mode = 0;
  score->delta_value = 0;
  score->loop_armed = 0;
  score->loop_count = 0;
  score->loop_on = 0;
  score->running_status = 0;
  score->mute_mask = 0;
  score->rhythm_n = 0;
  score->rhythm_d = 0;
  for (channel = 0; channel < 16; channel++)
  {
    score->programs[channel] = channel;
    score->panpot[channel] = 0x40;
    score->vol[channel] = 0x7F;
  }
  score->tick_wait = 1;
  score->read_pos = addr;
  if ((*score->read_pos == 'S') || (*score->read_pos == 'p'))
  {
    score->read_pos += 7;
    if (*score->read_pos++ != 1)
    {
      printf(D_80013874);
      return -1;
    }
  }
  else
  {
    printf(D_8001388C);
    return 0;
  }
  hi = *score->read_pos++;
  lo = *score->read_pos++;
  score->resolution = lo | (hi << 8);
  a = *score->read_pos++;
  b = *score->read_pos++;
  c = *score->read_pos++;
  score->hdr_tempo = c | ((a << 16) | (b << 8));
  if ((score->hdr_tempo / 2) < (60000000 % score->hdr_tempo))
  {
    score->hdr_tempo = (60000000 / score->hdr_tempo) + 1;
  }
  else
  {
    score->hdr_tempo = 60000000 / score->hdr_tempo;
  }
  AS(s32, score->tempo) = score->hdr_tempo;
  score->rhythm_n = *score->read_pos++;
  score->rhythm_d = *score->read_pos++;
  score->delta_value = score->start_delta = _SsReadDeltaValue(arg0, 0);
  score->start_pos = score->read_pos;
  score->loop_pos = score->read_pos;
  score->rewind_pos = score->read_pos;
  score->end_pos = 0;
  if ((score->resolution * score->hdr_tempo * 10) < (D_800E9624 * 60))
  {
    score->tick_step = score->tick_wait = (D_800E9624 * 600) / (score->resolution * score->hdr_tempo);
  }
  else
  {
    score->tick_wait = -1;
    score->tick_step = (score->resolution * score->hdr_tempo * 10) / (D_800E9624 * 60);
    if ((D_800E9624 * 30) < (score->resolution * score->hdr_tempo * 10) % (D_800E9624 * 60))
    {
      score->tick_step++;
    }
  }
  score->hdr_tick_step = score->tick_step;
  return 0;
}
INCLUDE_PAD(12);
