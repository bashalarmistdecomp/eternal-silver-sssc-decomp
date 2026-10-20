/* PSY-Q 4.5 libsnd, member vm_doff.o: _SsVmDamperOff. The first split had
   it in psyq_misc_q.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"

/* cc97_lib: _SsVmDamperOff */
/* return_slot_fill: _SsVmDamperOff */
extern u16 D_800EA688;    /* _svm_damper */

/* libsnd _SsVmDamperOff (VM_DOFF.OBJ): damper pedal off: _svm_damper = 0,
   the amount the voice manager adds to a voice's release rate when it
   allocates one (_SsVmDamperOn, sets 2). */
/* func_8009A238 */
void _SsVmDamperOff(void)
{
    D_800EA688 = 0;
}

INCLUDE_PAD(4);
