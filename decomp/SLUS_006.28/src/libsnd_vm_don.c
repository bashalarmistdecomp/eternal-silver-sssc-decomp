/* PSY-Q 4.5 libsnd, member vm_don.o: _SsVmDamperOn. The first split had it
   in psyq_misc_q.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
extern u16 D_800EA688;

extern u16 D_800EA688;

/* cc97_lib: _SsVmDamperOn */
/* return_slot_fill: _SsVmDamperOn */
/* libsnd _SsVmDamperOn (VM_DON.OBJ): damper pedal on: _svm_damper = 2 (see
   _SsVmDamperOff above). */
/* func_8009A248 */
void _SsVmDamperOn(void) {
    D_800EA688 = 2;
}
