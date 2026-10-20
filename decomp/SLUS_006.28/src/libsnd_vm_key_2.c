/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/vmanager.c), MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member vm_key.o, part 2 of 2: _SsVmSeKeyOn,
   _SsVmSeKeyOff, KeyOnCheck. The first split had it in psyq_misc_r.c. The
   tree compiles this run of the member's functions with other flags than
   the runs beside it (config/cc_flags.txt), so the member is in several
   files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: _SsVmSeKeyOn */
/* gnu_as_loads: _SsVmSeKeyOn */
s32 _SsVmKeyOn(s16, s16, s16, u16, u16, u16);

/* libsnd _SsVmSeKeyOn (VM_KEY.OBJ): key a note on directly, outside any
   score, from a left / right volume pair (the SsUtKeyOn path; nothing in
   this image calls it).  The pair becomes one volume and a pan for
   _SsVmKeyOn: equal volumes give the volume and pan 64; a louder left
   gives voll and pan = volr * 64 / voll (0..63); a louder right gives volr
   and pan = 127 - voll * 64 / volr (64..127).  The score is 0x21, the
   voice manager's "no score" number, and the result is _SsVmKeyOn's (the
   mask of the voices keyed on, or -1).  The fourth parameter is not used. */
/* func_8009B0F4 */
s32 _SsVmSeKeyOn(s16 vabId, s16 prog, u16 note, s32 unused, u16 voll, u16 volr)
{
    u16 pan;
    u16 vol;

    if (voll == volr) {
        pan = 64;
        vol = voll;
    } else if (volr < voll) {
        vol = voll;
        pan = (volr * 64) / voll;
    } else {
        vol = volr;
        pan = 127 - (voll * 64) / volr;
    }
    return _SsVmKeyOn(0x21, vabId, prog, note, vol, pan);
}
/* cc97_lib: _SsVmSeKeyOff */
/* gnu_as_loads: _SsVmSeKeyOff */
s32 _SsVmKeyOff(s16 seq_sep, s16 vab, s16 prog, u16 note);

/* libsnd _SsVmSeKeyOff (VM_KEY.OBJ): key a note off directly, outside any
   score (the SsUtKeyOff path; nothing in this image calls it):
   _SsVmKeyOff with the voice manager's "no score" number
   0x21, so every voice that _SsVmSeKeyOn started with this VAB, program
   and note is keyed off.  The result of _SsVmKeyOff is dropped. */
/* func_8009B1E0 */
void _SsVmSeKeyOff(s16 vabId, s16 prog, u16 note)
{
    _SsVmKeyOff(0x21, vabId, prog, note);
}
/* func_8009B214 */
void KeyOnCheck() {

}
INCLUDE_PAD(12);
