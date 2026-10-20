/* PSY-Q 4.5 libsnd, member ssstart.o, part 3 of 4: func_800983A8. The first
   split had it in psyq_misc_e.c. The tree compiles this run of the member's
   functions with other flags than the runs beside it (config/cc_flags.txt),
   so the member is in several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: func_800983A8 */
/* return_slot_fill: func_800983A8 */
extern SndSeqTickEnv D_800B79F0;

/* The handler _SsStart installs on interrupt 0 (VSync) in SS_TICKVSYNC mode
   with a root-counter request: run the VSync handler that was installed
   before it (vsync_cb, when there was one), then the tick function. */
void func_800983A8(void)
{
    if (D_800B79F0.vsync_cb != 0) {
        D_800B79F0.vsync_cb();
    }
    D_800B79F0.tick_cb();
}
