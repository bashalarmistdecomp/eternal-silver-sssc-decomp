/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/tempo.c) and psy-q-decomp (src/snd/tempo.c), all
   MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member tempo.o: _SsSndTempo. The first split had it in
   psyq_misc_l.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* The slice of a 0xB0-byte score record (_ss_score[a][b]) this object's
   functions use. */
extern SsScore *D_800E9628[];               
/* cc97_lib: _SsSndTempo */
extern u32 D_800E9624;                      /* VBLANK_MINUS */

/* libsnd _SsSndTempo (TEMPO.OBJ): one service tick of a tempo ramp
   (accelerando / ritardando) on the score _ss_score[seq_no][sep_no].
   SsSeqCalledTbyT calls it for a playing score while flag
   0x40 is set and again while 0x80 is set.
   tempo_count is counted down; below zero the ramp is over: both flags
   are cleared and nothing else changes.  Otherwise tempo moves toward
   tempo_goal: with tempo_step > 0 by one, on every tick whose count is a
   multiple of tempo_step (other ticks return at once); with tempo_step
   <= 0 by tempo_step each tick (the step is added when the tempo is above
   the goal and subtracted when below, so it is the negated size), clamped
   to the goal.  After a move tick_step is re-derived as resolution * tempo
   * 10 / (VBLANK_MINUS * 60), at least 1, and both flags are cleared when
   the count has reached zero or the tempo the goal. */
/* func_800998F8 */
void _SsSndTempo(short seq_no, short sep_no)
{
    SsScore *score;

    score = &D_800E9628[seq_no][sep_no];
    score->tempo_count--;
    if (score->tempo_count < 0) {
        D_800E9628[seq_no][sep_no].flags &= ~0x40;
        D_800E9628[seq_no][sep_no].flags &= ~0x80;
        return;
    }
    if (score->tempo_step > 0) {
        if (score->tempo_count % score->tempo_step == 0) {
            if (score->tempo > score->tempo_goal) {
                score->tempo--;
            } else if (score->tempo < score->tempo_goal) {
                score->tempo++;
            }
            score->tick_step = (score->resolution * score->tempo * 10) / (D_800E9624 * 60);
            if (score->tick_step <= 0) {
                score->tick_step = 1;
            }
            if (score->tempo_count == 0 || score->tempo == score->tempo_goal) {
                D_800E9628[seq_no][sep_no].flags &= ~0x40;
                D_800E9628[seq_no][sep_no].flags &= ~0x80;
            }
        }
    } else {
        if (score->tempo > score->tempo_goal) {
            score->tempo += score->tempo_step;
            if (score->tempo < score->tempo_goal) {
                score->tempo = score->tempo_goal;
            }
        } else if (score->tempo < score->tempo_goal) {
            score->tempo -= score->tempo_step;
            if (score->tempo > score->tempo_goal) {
                score->tempo = score->tempo_goal;
            }
        }
        score->tick_step = (score->resolution * score->tempo * 10) / (D_800E9624 * 60);
        if (score->tick_step <= 0) {
            score->tick_step = 1;
        }
        if (score->tempo_count == 0 || score->tempo == score->tempo_goal) {
            D_800E9628[seq_no][sep_no].flags &= ~0x40;
            D_800E9628[seq_no][sep_no].flags &= ~0x80;
        }
    }
}
INCLUDE_PAD(4);
