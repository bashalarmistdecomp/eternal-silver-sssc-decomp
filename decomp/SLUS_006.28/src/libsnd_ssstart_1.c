/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/ssstart.c), MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member ssstart.o, part 1 of 4: func_80098138. The first
   split had it in psyq_misc_c.c. The tree compiles this run of the member's
   functions with other flags than the runs beside it (config/cc_flags.txt),
   so the member is in several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: func_80098138 */
/* return_slot_fill: func_80098138 */
/* reorder_fill: func_80098138 */
extern SndSeqTickEnv D_800B79F0;
s32 EnterCriticalSection(void);
typedef void (*Callback)();     /* a libetc interrupt, DMA or VSync callback (libetc_intr_1.c's) */
extern Callback InterruptCallback(s32 irq, Callback f);
void func_800983A8(void);                           /* _SsTrapIntrVSync */
void func_800983F4(void);                           /* _SsSeqCalledTbyT_1per2 */

/* libsnd _SsStart (static, SSSTART.OBJ): install the sequencer tick that
   _snd_seq_tick_env (D_800B79F0) asks for.  SsStart calls
   it with 1, SsStart2 with 0.
   After a short busy wait the state is reset (no VSync ride, alarm on
   interrupt 6, root counter 2 = 0xF2000002, target 17640) and tick_mode
   picks the source: 0 (SS_NOTICK0) marks the alarm 127 and installs
   nothing; 5 (SS_TICKVSYNC) hangs on interrupt 0, riding VSyncCallback when
   use_vsync is 0 and root counter 3 (0xF2000003, target 1) otherwise; 3
   (SS_TICK120) is target 35280; 2 (SS_TICK240) keeps 17640; any other value
   is a rate in Hz, left alone when manual_tick is set: target 4233600 / hz,
   or below 70 Hz 2116800 / hz with the half-rate handler (unk11).
   The install runs inside a critical section: VSyncCallback(tick_cb), or
   ResetRCnt + SetRCnt(spec, target, 0x1000) and InterruptCallback on
   alarm_tick with the VSync trap func_800983A8 (interrupt 0; the handler it
   displaces is kept in vsync_cb), tick_cb, or the half-rate
   func_800983F4. */
void func_80098138(s32 use_vsync)
{
    u16 rcnt_target;
    s32 wait;
    u32 rcnt_spec;

    wait = 999;
    do {
        wait--;
    } while (wait >= 0);
    D_800B79F0.vsync_tick = 0;
    D_800B79F0.alarm_tick = 6;
    D_800B79F0.half_tick = 0;
    D_800B79F0.vsync_cb = 0;
    rcnt_spec = 0xF2000002;
    rcnt_target = 17640;
    switch (D_800B79F0.tick_mode) {
    case 0:
        D_800B79F0.alarm_tick = 127;
        return;
    case 5:
        D_800B79F0.alarm_tick = 0;
        if (use_vsync == 0) {
            D_800B79F0.vsync_tick = 1;
        } else {
            rcnt_spec = 0xF2000003;
            rcnt_target = 1;
        }
        break;
    case 3:
        rcnt_target = 35280;
        break;
    case 2:
        break;
    default:
        if (D_800B79F0.manual_tick) {
            return;
        }
        if (D_800B79F0.tick_mode < 70) {
            rcnt_target = 2116800 / D_800B79F0.tick_mode;
            D_800B79F0.half_tick++;
        } else {
            rcnt_target = 4233600 / D_800B79F0.tick_mode;
        }
        break;
    }

    if (D_800B79F0.vsync_tick) {
        EnterCriticalSection();
        VSyncCallback(D_800B79F0.tick_cb);
    } else {
        EnterCriticalSection();
        ResetRCnt(rcnt_spec);
        SetRCnt(rcnt_spec, rcnt_target, 0x1000);
        if (D_800B79F0.alarm_tick == 0) {
            D_800B79F0.vsync_cb = InterruptCallback(0, 0);
            InterruptCallback(D_800B79F0.alarm_tick, func_800983A8);
        } else if (D_800B79F0.half_tick == 0) {
            InterruptCallback(D_800B79F0.alarm_tick, D_800B79F0.tick_cb);
        } else {
            InterruptCallback(D_800B79F0.alarm_tick, func_800983F4);
        }
    }
    ExitCriticalSection();
}
