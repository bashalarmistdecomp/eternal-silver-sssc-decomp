/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/sscall.c), psyz (decomp/src/libsnd/sscall.c) and
   psy-q-decomp (src/snd/sscall.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member sscall.o: SsSeqCalledTbyT. The first split had
   it in psyq_misc_g.c.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* The slice of a 0xB0-byte score record (_ss_score[a][b]) this object's
   functions use. */
extern SsScore *D_800E9628[];   /* _ss_score */
/* cc97_lib: SsSeqCalledTbyT */
/* return_slot_fill: SsSeqCalledTbyT */
extern s32 D_800E961C;    /* _snd_ev_flag: 1 while a tick is running */
extern u32 D_800E9620;    /* _snd_openflag: bit a = access number a is open */
extern s16 D_800E9EA8;    /* _snd_seq_s_max: rows of _ss_score (SsSetTableSize) */
extern s16 D_800E9EAA;    /* _snd_seq_t_max: records per row */
void _SsVmFlush(void);
void _SsSndPlay(s16, s16);
void _SsSndCrescendo(s16 seq, s16 sep);
void _SsSndTempo(s16, s16);
void _SsSndPause(s16 seq, s16 sep);
void _SsSndReplay(s16, s16);
void _SsSndStop(s16, s16);

/* libsnd SsSeqCalledTbyT (SSCALL.OBJ): the sequencer tick (the function
   _snd_seq_tick_env.tick_cb points at; the VSync callback in this game).
   Re-entry is refused through _snd_ev_flag.
   One tick: flush the voice manager's pending register writes (_SsVmFlush),
   then for every open access number i (bit i of _snd_openflag) and every
   record j of its row serve the play-flag word: a playing score (1) reads
   its events (_SsSndPlay) and steps its volume ramp (0x10, 0x20:
   _SsSndCrescendo) and its tempo ramp (0x40, 0x80: _SsSndTempo); then,
   playing or not, a pause request (2: _SsSndPause), a replay request (8:
   _SsSndReplay) and a stop request (4: _SsSndStop, after which the whole
   flag word is cleared). */
/* func_80098448 */
void SsSeqCalledTbyT(void)
{
    int i;
    int j;

    if (D_800E961C != 1) {
        D_800E961C = 1;
        _SsVmFlush();
        for (i = 0; i < D_800E9EA8; i++) {
            if (D_800E9620 & (1 << i)) {
                for (j = 0; j < D_800E9EAA; j++) {
                    if (D_800E9628[i][j].flags & 1) {
                        _SsSndPlay(i, j);
                        if (D_800E9628[i][j].flags & 0x10) {
                            _SsSndCrescendo(i, j);
                        }
                        if (D_800E9628[i][j].flags & 0x20) {
                            _SsSndCrescendo(i, j);
                        }
                        if (D_800E9628[i][j].flags & 0x40) {
                            _SsSndTempo(i, j);
                        }
                        if (D_800E9628[i][j].flags & 0x80) {
                            _SsSndTempo(i, j);
                        }
                    }
                    if (D_800E9628[i][j].flags & 2) {
                        _SsSndPause(i, j);
                    }
                    if (D_800E9628[i][j].flags & 8) {
                        _SsSndReplay(i, j);
                    }
                    if (D_800E9628[i][j].flags & 4) {
                        _SsSndStop(i, j);
                        D_800E9628[i][j].flags = 0;
                    }
                }
            }
        }
        D_800E961C = 0;
    }
}
