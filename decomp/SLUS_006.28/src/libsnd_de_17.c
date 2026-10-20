/* PSY-Q 4.5 libsnd, member de_17.o: _SsSetNrpnVabAttr17. The first split
   had it in psyq_math_b.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

extern void SsUtSetReverbFeedback(s16);

/* cc97_lib: _SsSetNrpnVabAttr17 */

/* libsnd _SsSetNrpnVabAttr17(vabId, prog, tone, vagatr, fn_idx, data) (de_17.o): the NRPN
   data-entry handler for attribute 17 (DE_ECHO_FB), the reverb feedback:
   SsUtSetReverbFeedback(data).  Nothing else is used. */
/* func_80097508 */
void _SsSetNrpnVabAttr17(s16 vabId, s16 prog, s16 tone, VagAtr vagatr, s16 fn_idx, u8 data)
{
    SsUtSetReverbFeedback(data);
}

INCLUDE_PAD(12);
