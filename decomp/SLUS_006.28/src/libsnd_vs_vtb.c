/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/vs_vtb.c), psyz (decomp/src/libsnd/vs_vtb.c) and
   psy-q-decomp (src/snd/vs_vtb.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member vs_vtb.o: SsVabTransBody. The first split had it
   in psyq_misc_s.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* libsnd's per-voice state (_svm_voice[24], 0x38 each). */
/* libsnd's current-note parameters (_svm_cur). */
/* libsnd's per-score sequence state (_ss_score[seq][sep], 0xB0 each). */
extern u8 D_800EA710[];   /* _svm_vab_used[16] */
extern s32 D_800EA728[];
extern s32 D_800EA770[];
void _spu_setInTransfer(s32);

/* cc97_lib: SsVabTransBody */
/* gnu_as_loads: SsVabTransBody */
/* return_slot_fill: SsVabTransBody */
long SpuSetTransferMode(long);

/* libsnd SsVabTransBody: upload the sample body of VAB vabid, whose header
   SsVabOpenHead has opened (state 2), to the SPU RAM block reserved for it:
   SpuSetTransferMode(0), SpuSetTransferStartAddr(start), SpuWrite(addr,
   total); the slot's state becomes 1 and the VAB id is returned.  A bad id,
   a slot in another state or a refused start address clears the transfer
   flag (_spu_setInTransfer(0)) and returns -1. */
/* func_8009DBE8 */
s16 SsVabTransBody(u8 *addr, s16 vabid)
{
    s32 start;

    if (vabid >= 0 && vabid <= 16) {
        if (D_800EA710[vabid] == 2) {
            start = D_800EA770[vabid];
            SpuSetTransferMode(0);
            if (SpuSetTransferStartAddr(start) != 0) {
                SpuWrite(addr, D_800EA728[vabid]);
                D_800EA710[vabid] = 1;
                return vabid;
            }
        }
    }
    _spu_setInTransfer(0);
    return -1;
}
