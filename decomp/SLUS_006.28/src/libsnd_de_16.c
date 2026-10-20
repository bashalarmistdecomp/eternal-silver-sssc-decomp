/* PSY-Q 4.5 libsnd, member de_16.o: _SsSetNrpnVabAttr16. The first split
   had it in psyq_math_b.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

extern void SsUtSetReverbDepth(s16, s16);

/* cc97_lib: _SsSetNrpnVabAttr16 */

/* libsnd _SsSetNrpnVabAttr16(vabId, prog, tone, vagatr, fn_idx, data) (de_16.o): the NRPN
   data-entry handler for attribute 16 (DE_REV_DEPTH), the reverb depth:
   SsUtSetReverbDepth(data, data).  Nothing else is used. */
/* func_800974D8 */
void _SsSetNrpnVabAttr16(s16 vabId, s16 prog, s16 tone, VagAtr vagatr, s16 fn_idx, u8 data)
{
    SsUtSetReverbDepth(data, data);
}

INCLUDE_PAD(8);
