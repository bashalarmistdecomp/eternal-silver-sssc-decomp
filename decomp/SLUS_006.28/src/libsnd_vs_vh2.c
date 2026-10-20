/* PSY-Q 4.5 libsnd, member vs_vh2.o: SsVabOpenHead, func_8009D71C. The
   first split had it in psyq_misc_s.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* libsnd's per-voice state (_svm_voice[24], 0x38 each). */
/* libsnd's current-note parameters (_svm_cur). */
/* libsnd's per-score sequence state (_ss_score[seq][sep], 0xB0 each). */
extern u8 D_800EA710[];   /* _svm_vab_used[16] */
extern u16 D_800EA768;
void _spu_setInTransfer(s32);

/* cc97_lib: SsVabOpenHead */
s32 func_8009D71C(s32 size, s32 arg1, s16 vabId);  /* TU defines this below */
s16 _SsVabOpenHeadWithMode(u8 *addr, s16 vabid, s32 (*alloc)(s32, s32, s16), s32 sbaddr);  /* TU defines this below */

/* libsnd SsVabOpenHead (VS_VH2.OBJ): register the VAB header at addr in slot
   vabid (-1 = the first free one) and have the library allocate the SPU RAM
   for its samples: _SsVabOpenHeadWithMode with the object's
   own SpuMalloc helper (func_8009D71C) as the allocator and no address.
   Returns the slot, or -1. */
/* func_8009D6E8 */
s16 SsVabOpenHead(u8 *addr, s16 vabid)
{
    return _SsVabOpenHeadWithMode(addr, vabid, func_8009D71C, 0);
}

/* cc272_lib: func_8009D71C */
/* return_slot_fill: func_8009D71C */

s32 func_8009D71C(s32 size, s32 arg1, s16 vabId)
{
    s32 addr;

    addr = SpuMalloc(size);
    if (addr == -1) {
        D_800EA710[vabId] = 0;
        _spu_setInTransfer(0);
        D_800EA768--;
        return -1;
    }
    return addr;
}

INCLUDE_PAD(4);
