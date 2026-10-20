/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/ssend.c), psyz (decomp/src/libsnd/ssend.c) and
   psy-q-decomp (src/snd/ssend.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member ssend.o: SsEnd. The first split had it in
   psyq_spu_m.c.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: SsEnd */
/* return_slot_fill: SsEnd */
extern SndSeqTickEnv D_800B79F0;    /* _snd_seq_tick_env */
s32 EnterCriticalSection(void);
typedef void (*Callback)();     /* a libetc interrupt, DMA or VSync callback (libetc_intr_1.c's) */
extern Callback InterruptCallback(s32 irq, Callback f);

/* libsnd SsEnd (SSEND.OBJ): take the sequencer tick that _SsStart (func_80098138)
   installed out again. Nothing happens when the program ticks by hand (manual_tick).
   Otherwise the half-rate flag (unk11) is cleared and, unless no tick is installed
   (alarm_tick == 127), inside a critical section: a tick riding VSyncCallback
   (vsync_tick) is removed with VSyncCallback(0); one hanging on interrupt 0
   (alarm_tick == 0) gives the interrupt back to the VSync handler _SsStart displaced
   (vsync_cb), which is then forgotten; any other one is removed with
   InterruptCallback(6, 0). alarm_tick becomes 127. */
/* func_80095928 */
void SsEnd(void)
{
    if (D_800B79F0.manual_tick == 0) {
        D_800B79F0.half_tick = 0;
        if (D_800B79F0.alarm_tick != 127) {
            EnterCriticalSection();
            if (D_800B79F0.vsync_tick != 0) {
                VSyncCallback(0);
                D_800B79F0.vsync_tick = 0;
            } else if (D_800B79F0.alarm_tick == 0) {
                InterruptCallback(0, D_800B79F0.vsync_cb);
                D_800B79F0.vsync_cb = 0;
            } else {
                InterruptCallback(6, 0);
            }
            ExitCriticalSection();
            D_800B79F0.alarm_tick = 127;
        }
    }
}
INCLUDE_PAD(4);
