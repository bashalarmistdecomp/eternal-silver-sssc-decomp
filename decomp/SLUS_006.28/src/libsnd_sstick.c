/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/sstick.c), psyz (decomp/src/libsnd/sstick.c) and
   psy-q-decomp (src/snd/sstick.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member sstick.o: SsSetTickMode. The first split had it
   in psyq_misc_k.c.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: SsSetTickMode */
/* return_slot_fill: SsSetTickMode */
extern SndSeqTickEnv D_800B79F0;    /* _snd_seq_tick_env */
extern s32 D_800E9624;              /* VBLANK_MINUS */

/* libsnd SsSetTickMode (SSTICK.OBJ): choose the sound service's tick.
   SS_NOTICK (0x1000) in tick_mode sets manual_tick (the program calls
   SsSeqCalledTbyT itself) and is masked off.  The mode left is stored in
   _snd_seq_tick_env.tick_mode and decides VBLANK_MINUS, the ticks per
   second the SEQ reader divides by:
     SS_TICK50 (4)     50; tick_mode becomes SS_TICKVSYNC on a PAL
                       console (video mode 1), else the rate 50
     SS_TICK60 (1)     60; tick_mode becomes SS_TICKVSYNC on an NTSC
                       console (video mode 0), else the rate 60
     SS_TICK120 (3)    120
     SS_TICK240 (2)    240
     SS_TICKVSYNC (5), SS_NOTICK0 (0)
                       the console's field rate: 50 for PAL, else 60
     6 and up          the value itself, a rate in ticks per second
     negative          60 (the switch's default) */
/* func_800995E8 */
void SsSetTickMode(long tick_mode)
{
    int video_mode;

    video_mode = GetVideoMode();
    if (tick_mode & 0x1000) {
        D_800B79F0.manual_tick = 1;
        D_800B79F0.tick_mode = tick_mode & 0xFFF;
    } else {
        D_800B79F0.manual_tick = 0;
        D_800B79F0.tick_mode = tick_mode;
    }
    if (D_800B79F0.tick_mode < 6) {
        switch (D_800B79F0.tick_mode) {
        case 4:
            D_800E9624 = 50;
            if (video_mode != 1) {
                D_800B79F0.tick_mode = 50;
            } else {
                D_800B79F0.tick_mode = 5;
            }
            return;
        case 1:
            D_800E9624 = 60;
            if (video_mode == 0) {
                D_800B79F0.tick_mode = 5;
            } else {
                D_800B79F0.tick_mode = 60;
            }
            return;
        case 3:
            D_800E9624 = 120;
            return;
        case 2:
            D_800E9624 = 240;
            return;
        case 5:
            if (video_mode == 0) {
                D_800E9624 = 60;
            } else if (video_mode == 1) {
                D_800E9624 = 50;
            } else {
                D_800E9624 = 60;
            }
            break;
        case 0:
            if (video_mode == 0) {
                D_800E9624 = 60;
            } else if (video_mode == 1) {
                D_800E9624 = 50;
            } else {
                D_800E9624 = 60;
            }
            return;
        default:
            D_800E9624 = 60;
            return;
        }
    } else {
        D_800E9624 = D_800B79F0.tick_mode;
    }
}
INCLUDE_PAD(8);

INCLUDE_RODATA("asm/data/jtbl", jtbl_800138B4_pad);
