/* PSY-Q 4.5 libsnd, member cc_64.o: _SsContDamper. The first split had it
   in psyq_math.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"

/* libsnd's score record (0xB0 bytes, one per sequence: _ss_score[seq_no][sep_no]); the
   members the SEQ control-change handlers of this TU use. */

/* The nine shorts a tone's two ADSR words are split into (ccadsr.o's record:
   _SsUtResolveADSR fills it, _SsUtBuildADSR packs it back).  _SsUtBuildADSR reads the
   five rates unsigned and the four mode flags signed. */

extern SsScore *D_800E9628[];                    /* _ss_score */
s32 _SsReadDeltaValue(s16 seq_no, s16 sep_no);
void _SsVmDamperOff(void);
void _SsVmDamperOn(void);

/* cc97_lib: _SsContDamper */
/* gnu_as_loads: _SsContDamper */
/* return_slot_fill: _SsContDamper */

/* libsnd _SsContDamper(seq_no, sep_no, data) (cc_64.o): SEQ control change 0x40 (sustain
   pedal): data < 0x40 calls _SsVmDamperOff, otherwise _SsVmDamperOn
   (it sets D_800EA688 = 2); then on score = &_ss_score[seq_no][sep_no]
   (D_800E9628) delta_value = _SsReadDeltaValue.  The recorded size ends
   in three pad nops, hence INCLUDE_PAD(12). */
/* func_80096458 */
void _SsContDamper(s16 seq_no, s16 sep_no, u8 data)
{
    SsScore *score;

    score = &D_800E9628[seq_no][sep_no];
    if (data < 0x40) {
        _SsVmDamperOff();
    } else {
        _SsVmDamperOn();
    }
    score->delta_value = _SsReadDeltaValue(seq_no, sep_no);
}

INCLUDE_PAD(12);
