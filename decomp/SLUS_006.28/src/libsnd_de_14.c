/* PSY-Q 4.5 libsnd, member de_14.o: _SsSetNrpnVabAttr14. The first split
   had it in psyq_math.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"

/* The nine shorts a tone's two ADSR words are split into (ccadsr.o's record:
   _SsUtResolveADSR fills it, _SsUtBuildADSR packs it back).  _SsUtBuildADSR reads the
   five rates unsigned and the four mode flags signed. */

s16 SsUtGetVagAtr(s16 vabId, s16 progNum, s16 toneNum, VagAtr *vagatrptr);
s16 SsUtSetVagAtr(s16 vabId, s16 progNum, s16 toneNum, VagAtr *vagatrptr);
void _SsUtBuildADSR(SsAdsr *adsr, u16 *adsr1, u16 *adsr2);

/* cc97_lib: _SsSetNrpnVabAttr14 */
/* return_slot_fill: _SsSetNrpnVabAttr14 */

/* libsnd _SsSetNrpnVabAttr14(vabId, prog, tone, vagatr, fn_idx, data) (de_14.o): the NRPN
   data-entry handler for tone attribute 14, the portamento depth.  The by-value VagAtr is
   scratch: SsUtGetVagAtr reads the tone's attributes into it, porW (+0x0A)
   = data, _SsUtBuildADSR packs the nine ADSR shorts into adsr1 / adsr2
   (+0x10 / +0x12) and SsUtSetVagAtr writes the tone back.  As in
   _SsSetNrpnVabAttr13 the nine shorts are never filled in (no _SsUtResolveADSR call), so
   the tone's ADSR words are rebuilt from whatever the stack held.  fn_idx is not used. */
/* func_80097408 */
void _SsSetNrpnVabAttr14(s16 vabId, s16 prog, s16 tone, VagAtr vagatr, s16 fn_idx, u8 data)
{
    SsAdsr adsr;

    SsUtGetVagAtr(vabId, prog, tone, &vagatr);
    vagatr.porW = data;
    _SsUtBuildADSR(&adsr, &vagatr.adsr1, &vagatr.adsr2);
    SsUtSetVagAtr(vabId, prog, tone, &vagatr);
}
INCLUDE_PAD(4);
