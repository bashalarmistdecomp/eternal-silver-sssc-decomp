/* PSY-Q 4.5 libsnd, member vs_vab.o: SsVabClose. The first split had it in
   psyq_misc_s.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* libsnd's per-voice state (_svm_voice[24], 0x38 each). */
/* libsnd's current-note parameters (_svm_cur). */
/* libsnd's per-score sequence state (_ss_score[seq][sep], 0xB0 each). */
extern u8 D_800EA710[];   
/* cc272_lib: SsVabClose */
/* return_slot_fill: SsVabClose */
/* gnu_as_loads: SsVabClose */
extern s32 D_800EA770[];  /* _svm_vab_start[16] */
extern u16 D_800EA768;    /* _svm_vab_count */
void SpuFree(unsigned long);
void _spu_setInTransfer(s32);
s32 _spu_getInTransfer(void);

/* func_8009D638 */
void SsVabClose(s16 vabId)
{
    s32 used;

    if ((u16)vabId < 16) {
        used = D_800EA710[vabId];
        if (used < 3 && used != 0) {
            SpuFree(D_800EA770[vabId]);
            D_800EA710[vabId] = 0;
            D_800EA768--;
            if (_spu_getInTransfer() == 1) {
                _spu_setInTransfer(0);
            }
        }
    }
}

INCLUDE_PAD(12);
