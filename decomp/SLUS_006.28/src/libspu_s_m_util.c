/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libspu/s_m_util.c) and psyz (decomp/src/libspu/s_m_util.c),
   all MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libspu, member s_m_util.o: _SpuIsInAllocateArea,
   _SpuIsInAllocateArea_. The first split had it in psyq_misc_af.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
s32 _SpuIsInAllocateArea_(u32);

/* cc97_lib: _SpuIsInAllocateArea */
/* far_syms: D_800B7C18 */
extern SPU_MALLOC *D_800B7C18;  /* _spu_memList */

/* libspu _SpuIsInAllocateArea (S_M_UTIL.OBJ): 1 when the SPU address lies at or below the
   start of an allocated block or inside one, 0 when no block is in the way (or SpuInitMalloc
   was never called) */
/* func_8009F508 */
s32 _SpuIsInAllocateArea(u32 addr) {
    s32 i;

    if (D_800B7C18 == 0) {
        return 0;
    }
    for (i = 0; 1; i++) {
        if (D_800B7C18[i].addr & 0x80000000) {
            continue;
        }
        if (D_800B7C18[i].addr & 0x40000000) {
            break;
        }
        if (addr <= (D_800B7C18[i].addr & 0x0FFFFFFF)) {
            return 1;
        }
        if (addr < (D_800B7C18[i].addr & 0x0FFFFFFF) + D_800B7C18[i].size) {
            return 1;
        }
    }
    return 0;
}
/* cc97_lib: _SpuIsInAllocateArea_ */
/* far_syms: D_800B7BD8 */
extern s32 D_800B7BD8;          /* _spu_mem_mode_plus */

/* libspu _SpuIsInAllocateArea_ (S_M_UTIL.OBJ): _SpuIsInAllocateArea for an address given in
   SPU register units (shifted left by _spu_mem_mode_plus first) */
/* func_8009F588 */
s32 _SpuIsInAllocateArea_(u32 addr) {
    s32 i;

    addr <<= D_800B7BD8;
    if (D_800B7C18 == 0) {
        return 0;
    }
    for (i = 0; 1; i++) {
        if (D_800B7C18[i].addr & 0x80000000) {
            continue;
        }
        if (D_800B7C18[i].addr & 0x40000000) {
            break;
        }
        if (addr <= (D_800B7C18[i].addr & 0x0FFFFFFF)) {
            return 1;
        }
        if (addr < (D_800B7C18[i].addr & 0x0FFFFFFF) + D_800B7C18[i].size) {
            return 1;
        }
    }
    return 0;
}

INCLUDE_PAD(4);
