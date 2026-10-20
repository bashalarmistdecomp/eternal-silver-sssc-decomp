/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/vmanager.c) and psyz (decomp/src/libsnd/vm_aloc1.c),
   all MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member vm_aloc1.o: _SsVmAlloc. The first split had it
   in psyq_misc_s.c.
   The build must stay byte-matching. */

/* far_syms: D_800B79C0 (target reaches it %hi/%lo) */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* libsnd's per-voice state (_svm_voice[24], 0x38 each). */
/* libsnd's current-note parameters (_svm_cur). */
/* libsnd's per-score sequence state (_ss_score[seq][sep], 0xB0 each). */
extern u32 D_800B79C0;      /* _snd_vmask */
extern SvmVoice D_800E9EC0[];  /* _svm_voice[24] */
extern s8 D_800EA6E4;       /* _SsVmMaxVoice */
extern SvmCur D_800EA6F0;   /* _svm_cur */
/* cc97_lib: _SsVmAlloc */
/* gnu_as_loads: _SsVmAlloc */

/* libsnd _SsVmAlloc (VM_ALOC1.OBJ): pick the SPU voice for the note in
   _svm_cur.  Voices whose bit is set in _snd_vmask are skipped.  An idle
   voice (key state 0 and unk6 0) is taken at once.  Otherwise the victim
   is the voice of lowest priority, no higher than the new note's; ties go
   to the lowest unk6, then to the greatest age.  With no idle voice and
   none of lower or equal priority the result is _SsVmMaxVoice (no voice).
   On success every unmasked voice ages by one and the chosen one restarts
   at age 0 with the note's priority and unk1E / unk2A cleared.  The
   argument is unused (the caller passes 0). */
/* func_8009B228 */
u8 _SsVmAlloc(s32 arg0)
{
    u8 channel;
    u16 minkey;
    u8 count;
    u16 age;
    u8 best;
    u8 i;
    u16 prior;

    channel = 99;
    minkey = 0xFFFF;
    count = 0;
    age = 0;
    best = 99;
    prior = D_800EA6F0.prior;
    for (i = 0; i < D_800EA6E4; i++) {
        if ((D_800B79C0 & (1 << i)) == 0) {
            if (D_800E9EC0[i].key_stat == 0 && D_800E9EC0[i].envx == 0) {
                channel = i;
                break;
            }
            if (D_800E9EC0[i].prior < prior) {
                prior = D_800E9EC0[i].prior;
                best = i;
                minkey = D_800E9EC0[i].envx;
                age = D_800E9EC0[i].age;
                count = 1;
            } else if (D_800E9EC0[i].prior == prior) {
                count++;
                if (D_800E9EC0[i].envx < minkey) {
                    age = D_800E9EC0[i].age;
                    minkey = D_800E9EC0[i].envx;
                    best = i;
                } else if (D_800E9EC0[i].envx == minkey) {
                    if (age < D_800E9EC0[i].age) {
                        age = D_800E9EC0[i].age;
                        best = i;
                    }
                }
            }
        }
    }
    if (channel == 99) {
        if (count == 0) {
            channel = D_800EA6E4;
        } else {
            channel = best;
        }
    }
    if (channel < D_800EA6E4) {
        for (i = 0; i < D_800EA6E4; i++) {
            if ((D_800B79C0 & (1 << i)) == 0) {
                D_800E9EC0[i].age += 1;
            }
        }
        D_800E9EC0[channel].age = 0;
        D_800E9EC0[channel].prior = D_800EA6F0.prior;
        D_800E9EC0[channel].auto_pan = 0;
        D_800E9EC0[channel].auto_vol = 0;
    }
    return channel;
}
