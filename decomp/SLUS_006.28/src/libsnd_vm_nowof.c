/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/vmanager.c) and psyz (decomp/src/libsnd/vm_nowof.c),
   all MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member vm_nowof.o: _SsVmKeyOffNow. The first split had
   it in psyq_misc_s.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* libsnd's per-voice state (_svm_voice[24], 0x38 each). */
/* libsnd's current-note parameters (_svm_cur). */
/* libsnd's per-score sequence state (_ss_score[seq][sep], 0xB0 each). */
extern SvmVoice D_800E9EC0[];  /* _svm_voice[24] */
extern SvmCur D_800EA6F0;
extern u16 D_800E9EB0;
extern u16 D_800E9EB2;
extern u16 D_800EA5E8;
extern u16 D_800EA5EA;

/* cc97_lib: _SsVmKeyOffNow */
/* gnu_as_loads: _SsVmKeyOffNow */
/* return_slot_fill: _SsVmKeyOffNow */

/* libsnd _SsVmKeyOffNow (VM_NOWOF.OBJ): key off the voice _svm_cur.voice at
   once.  Its record goes idle (key state +0x1D, the pitch word +0x04 and
   the VAG number +0x00 = 0), its bit is set in the pending key-off masks
   (_svm_okof1/2, D_800EA5E8 / D_800EA5EA; voices 0-15 in the first word,
   16-23 in the second) and every pending key-off is cleared from the
   pending key-on masks (_svm_okon1/2, D_800E9EB0 / D_800E9EB2).  The
   argument is never read; the callers pass 0 or 1. */
/* func_8009BDF8 */
void _SsVmKeyOffNow(s32 mode)
{
    s32 bitsUpper;
    s32 bitsLower;
    u16 voice;

    voice = D_800EA6F0.voice;
    if (voice < 16) {
        bitsLower = 1 << voice;
        bitsUpper = 0;
    } else {
        bitsLower = 0;
        bitsUpper = 1 << (voice - 16);
    }
    D_800E9EC0[voice].key_stat = 0;
    D_800E9EC0[voice].pitch = 0;
    D_800E9EC0[voice].vag = 0;
    D_800EA5E8 |= bitsLower;
    D_800EA5EA |= bitsUpper;
    D_800E9EB0 &= ~D_800EA5E8;
    D_800E9EB2 &= ~D_800EA5EA;
}
