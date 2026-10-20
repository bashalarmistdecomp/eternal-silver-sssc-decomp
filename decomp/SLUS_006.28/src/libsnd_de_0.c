/* PSY-Q 4.5 libsnd, member de_0.o: _SsSetNrpnVabAttr0. The first split had
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

/* cc97_lib: _SsSetNrpnVabAttr0 */
/* return_slot_fill: _SsSetNrpnVabAttr0 */

/* libsnd _SsSetNrpnVabAttr0(vabId, prog, tone, vagatr, fn_idx, data) (de_0.o): the NRPN
   data-entry handler for tone attribute 0, the priority.  The VagAtr parameter is passed
   by value and used as scratch: SsUtGetVagAtr reads the tone's attributes
   into it, prior (+0x00) = data, SsUtSetVagAtr writes them back.  fn_idx
   is not used. */
/* func_80096978 */
void _SsSetNrpnVabAttr0(s16 vabId, s16 prog, s16 tone, VagAtr vagatr, s16 fn_idx, u8 data)
{
    SsUtGetVagAtr(vabId, prog, tone, &vagatr);
    vagatr.prior = data;
    SsUtSetVagAtr(vabId, prog, tone, &vagatr);
}

INCLUDE_PAD(4);
