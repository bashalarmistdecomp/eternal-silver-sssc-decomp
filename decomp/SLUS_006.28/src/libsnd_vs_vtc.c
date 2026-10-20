/* PSY-Q 4.5 libsnd, member vs_vtc.o: SsVabTransCompleted. The first split
   had it in psyq_misc_s.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* libsnd's per-voice state (_svm_voice[24], 0x38 each). */
/* libsnd's current-note parameters (_svm_cur). */

/* cc97_lib: SsVabTransCompleted */
/* return_slot_fill: SsVabTransCompleted */
long SpuIsTransferCompleted(long flag);

/* libsnd SsVabTransCompleted (VS_VTC.OBJ): has the SPU finished the
   transfer SsVabTransBody started?  SpuIsTransferCompleted(immediateFlag)
   , its result cut to a short: 1 = finished; with
   SS_WAIT_COMPLETED (1) the call waits for the end. */
/* func_8009DCA8 */
s16 SsVabTransCompleted(s16 immediateFlag)
{
    return SpuIsTransferCompleted(immediateFlag);
}
INCLUDE_PAD(8);
