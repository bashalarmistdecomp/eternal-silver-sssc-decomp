/* PSY-Q 4.5 libsnd, member de_1.o: _SsSetNrpnVabAttr1. The first split had
   it in psyq_math.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"

/* The nine shorts a tone's two ADSR words are split into (ccadsr.o's record:
   _SsUtResolveADSR fills it, _SsUtBuildADSR packs it back).  _SsUtBuildADSR reads the
   five rates unsigned and the four mode flags signed. */

s16 SsUtGetVagAtr(s16 vabId, s16 progNum, s16 toneNum, VagAtr *vagatrptr);
s16 SsUtSetVagAtr(s16 vabId, s16 progNum, s16 toneNum, VagAtr *vagatrptr);

/* cc97_lib: _SsSetNrpnVabAttr1 */
/* return_slot_fill: _SsSetNrpnVabAttr1 */

/* libsnd _SsSetNrpnVabAttr1(vabId, prog, tone, vagatr, fn_idx, data) (de_1.o): the NRPN
   data-entry handler for tone attribute 1, the mode.  The by-value VagAtr is scratch:
   SsUtGetVagAtr reads the tone's attributes into it, mode (+0x01) = data,
   SsUtSetVagAtr writes them back.  Then the reverb follows the mode:
   0 = SsUtReverbOff, 4 = SsUtReverbOn, any
   other value leaves it alone.  fn_idx is not used. */
/* func_80096A08 */
void _SsSetNrpnVabAttr1(s16 vabId, s16 prog, s16 tone, VagAtr vagatr, s16 fn_idx, u8 data)
{
    SsUtGetVagAtr(vabId, prog, tone, &vagatr);
    vagatr.mode = data;
    SsUtSetVagAtr(vabId, prog, tone, &vagatr);
    if (data == 0) {
        SsUtReverbOff();
    } else if (data == 4) {
        SsUtReverbOn();
    }
}

INCLUDE_PAD(8);
