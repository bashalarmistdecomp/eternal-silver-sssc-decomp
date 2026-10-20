/* PSY-Q 4.5 libsnd, member de_18.o: _SsSetNrpnVabAttr18. The first split
   had it in psyq_math_b.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

extern void SsUtSetReverbDelay(s16);

/* cc97_lib: _SsSetNrpnVabAttr18 */

/* libsnd _SsSetNrpnVabAttr18(vabId, prog, tone, vagatr, fn_idx, data) (de_18.o): the NRPN
   data-entry handler for attribute 18 (DE_ECHO_DELAY), the reverb delay:
   SsUtSetReverbDelay(data).  Nothing else is used. */
/* func_80097538 */
void _SsSetNrpnVabAttr18(s16 vabId, s16 prog, s16 tone, VagAtr vagatr, s16 fn_idx, u8 data)
{
    SsUtSetReverbDelay(data);
}

INCLUDE_PAD(12);
