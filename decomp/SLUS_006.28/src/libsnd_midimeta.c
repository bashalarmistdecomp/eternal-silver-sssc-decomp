/* PSY-Q 4.5 libsnd, member midimeta.o: _SsGetMetaEvent. The first split had
   it in psyq_math_c.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"

/* cc97_lib: _SsSetPitchBend */
/* gnu_as_loads: _SsSetPitchBend */
/* return_slot_fill: _SsSetPitchBend */
/* libsnd's per-score sequence state (_ss_score[seq][sep], 0xB0 bytes each;
   psyz's and sotn-decomp's struct SeqStruct, at this library version's
   offsets).  The names are docs/spec/code/subsystems/psyq-libs.md's. */
extern SsScore *D_800E9628[];  /* _ss_score */
s32 _SsReadDeltaValue(s16, s16);

/* cc97_lib: _SsGetMetaEvent */
/* gnu_as_loads: _SsGetMetaEvent */
/* return_slot_fill: _SsGetMetaEvent */
extern u32 D_800E9624;

/* libsnd _SsGetMetaEvent(seq_no, sep_no, meta type) (midimeta.o; instruction-identical
   to the 4.6 and 4.7 library objects): the SEQ reader's tempo meta event (FF 51). The
   reader (_SsGetSeqData) calls it through SsFCALL.metaevent (D_800E9594) for every meta
   type but 0x2F and leaves the type byte in the third argument, which is never read.
   score = &_ss_score[seq_no][sep_no] (D_800E9628, 0xB0-byte records). Takes the three
   big-endian bytes at the read pointer (microseconds per quarter note), stores tempo =
   60000000 / that, and re-derives the step _SsSeqPlay advances the
   score by, as _SsInitSoundSeq does for the header's tempo: with n =
   resolution * tempo (delta ticks per minute) and VBLANK_MINUS (D_800E9624) the sound
   service's ticks per second, n * 10 < VBLANK_MINUS * 60 gives tick_wait = tick_step =
   VBLANK_MINUS * 600 / n (the countdown path); otherwise tick_wait = -1 and tick_step =
   n * 10 / (VBLANK_MINUS * 60) rounded to nearest, the delta time one service tick
   takes, in the tenths of a delta tick _SsReadDeltaValue counts in.
   Then delta_value = _SsReadDeltaValue. The recorded size ends in two pad nops, hence
   INCLUDE_PAD(8). */
/* func_80097878 */
void _SsGetMetaEvent(s16 arg0, s16 arg1, u8 arg2)
{
  SsScore *score;
  s32 a;
  s32 b;
  s32 c;
  score = &D_800E9628[arg0][arg1];
  a = *score->read_pos++;
  b = *score->read_pos++;
  c = *score->read_pos++;
  AS(s32, score->tempo) = 60000000 / (c | ((a << 16) | (b << 8)));
  if ((score->resolution * AS(s32, score->tempo) * 10) < D_800E9624 * 60)
  {
    score->tick_step = score->tick_wait = (D_800E9624 * 600) / (score->resolution * AS(s32, score->tempo));
  }
  else
  {
    score->tick_wait = -1;
    score->tick_step = (score->resolution * AS(s32, score->tempo) * 10) / (D_800E9624 * 60);
    if ((D_800E9624 * 30) < (score->resolution * AS(s32, score->tempo) * 10) % (D_800E9624 * 60))
    {
      score->tick_step++;
    }
  }
  score->delta_value = _SsReadDeltaValue(arg0, arg1);
}
INCLUDE_PAD(8);
