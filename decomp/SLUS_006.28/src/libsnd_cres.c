/* PSY-Q 4.5 libsnd, member cres.o: _SsSndCrescendo. The first split had it
   in psyq_misc_g.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* The slice of a 0xB0-byte score record (_ss_score[a][b]) this object's
   functions use. */
extern SsScore *D_800E9628[];   /* _ss_score */
/* cc97_lib: SsSeqCalledTbyT */
/* return_slot_fill: SsSeqCalledTbyT */
void _SsSndCrescendo(s16 seq, s16 sep);

/* cc97_lib: _SsSndCrescendo */
/* gnu_as_loads: _SsSndCrescendo */
/* return_slot_fill: _SsSndCrescendo */
s16 _SsVmSetSeqVol(s16, u16, u16, s16);
s16 _SsVmGetSeqVol(s16, u16 *, u16 *);

/* libsnd _SsSndCrescendo (CRES.OBJ): one tick of a score's volume ramp; the
   tick calls it while flag 0x10 is set and again while 0x20 is set.
   cres_tick is counted up; past cres_time the ramp is over and flag 0x10 is
   cleared.  Otherwise d = the change due by now (cres_vol * cres_tick /
   cres_time) less the change already made (cres_done); when d is not zero
   it is added to cres_done and to both volumes of the score
   (_SsVmGetSeqVol, clamped to 0..127, _SsVmSetSeqVol), and the flag is
   cleared once both are 127 or both are 0.  In every case the volume pair
   is read back into cres_voll / cres_volr. */
/* func_800986B8 */
void _SsSndCrescendo(s16 seq, s16 sep)
{
    SsScore *score = &D_800E9628[seq][sep];
    u16 voll, volr;
    int d, l, r;

    score->cres_tick++;
    if (score->cres_time < score->cres_tick) {
        D_800E9628[seq][sep].flags &= ~0x10;
    } else {
        d = score->cres_vol * score->cres_tick / score->cres_time;
        d -= score->cres_done;
        if (d != 0) {
            score->cres_done += d;
            _SsVmGetSeqVol(seq | (sep << 8), &voll, &volr);
            l = voll + d;
            if (l > 127) {
                l = 127;
            }
            if (l < 0) {
                l = 0;
            }
            r = volr + d;
            if (r > 127) {
                r = 127;
            }
            if (r < 0) {
                r = 0;
            }
            _SsVmSetSeqVol(seq | (sep << 8), l, r, 1);
            if ((l == 127 && r == 127) || (l == 0 && r == 0)) {
                D_800E9628[seq][sep].flags &= ~0x10;
            }
        }
    }
    _SsVmGetSeqVol(seq | (sep << 8), &score->cres_voll, &score->cres_volr);
}

INCLUDE_PAD(8);
