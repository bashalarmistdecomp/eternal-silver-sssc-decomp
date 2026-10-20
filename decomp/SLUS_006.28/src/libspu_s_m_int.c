/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libspu/s_m_m.c) and psyz (decomp/src/libspu/s_m_int.c), all
   MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libspu, member s_m_int.o: _spu_gcSPU. The first split had it in
   psyq_misc_ab.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"

/* cc97_lib: _spu_gcSPU */
/* far_syms: D_800B7C14 */
/* far_syms: D_800B7C18 */
extern s32 D_800B7C14;          /* _spu_AllocLastNum: index of the last record */
extern SPU_MALLOC *D_800B7C18;  /* _spu_memList */
/* cc97_lib: SpuMalloc */
/* return_slot_fill: SpuMalloc */
void _spu_gcSPU(void);

/* libspu _spu_gcSPU (S_M_INT.OBJ)
   MARKED SPELLING: the first pass's inner loop is `for (j = i + 1; 1; j++)`, a constant
   condition with the exit as the `break` in its body. It is sotn-decomp's spelling of
   this loop (`for (j = i + 1; true; j++)`) and semantically inert; with no condition
   (`for (j = i + 1;; j++)`, psyz's spelling) the exit test is rolled to the loop's bottom
   and the function scores 177/192. The original construct is not known. */
/* func_8009ED18 */
void _spu_gcSPU(void) {
    s32 i;
    s32 j;

    for (i = 0; i <= D_800B7C14;) {
        if (D_800B7C18[i].addr & 0x80000000) {
            for (j = i + 1; 1; j++) {
                if (D_800B7C18[j].addr != 0x2FFFFFFF) {
                    break;
                }
            }
            if ((D_800B7C18[j].addr & 0x80000000) &&
                ((D_800B7C18[j].addr & 0x0FFFFFFF) ==
                 ((D_800B7C18[i].addr & 0x0FFFFFFF) + D_800B7C18[i].size))) {
                D_800B7C18[j].addr = 0x2FFFFFFF;
                D_800B7C18[i].size += D_800B7C18[j].size;
                continue;
            }
        }
        i++;
    }
    for (i = 0; i <= D_800B7C14; i++) {
        if (D_800B7C18[i].size == 0) {
            D_800B7C18[i].addr = 0x2FFFFFFF;
        }
    }
    for (i = 0; i <= D_800B7C14; i++) {
        if (D_800B7C18[i].addr & 0x40000000) {
            break;
        }
        for (j = i + 1; j <= D_800B7C14; j++) {
            if (D_800B7C18[j].addr & 0x40000000) {
                break;
            }
            if ((D_800B7C18[j].addr & 0x0FFFFFFF) < (D_800B7C18[i].addr & 0x0FFFFFFF)) {
                u32 swapAddr = D_800B7C18[i].addr;
                u32 swapSize = D_800B7C18[i].size;
                D_800B7C18[i].addr = D_800B7C18[j].addr;
                D_800B7C18[i].size = D_800B7C18[j].size;
                D_800B7C18[j].addr = swapAddr;
                D_800B7C18[j].size = swapSize;
            }
        }
    }
    for (i = 0; i <= D_800B7C14; i++) {
        if (D_800B7C18[i].addr & 0x40000000) {
            break;
        }
        if (D_800B7C18[i].addr == 0x2FFFFFFF) {
            D_800B7C18[i].addr = D_800B7C18[D_800B7C14].addr;
            D_800B7C18[i].size = D_800B7C18[D_800B7C14].size;
            D_800B7C14 = i;
            break;
        }
    }
    for (i = D_800B7C14 - 1; i >= 0; i--) {
        if (!(D_800B7C18[i].addr & 0x80000000)) {
            break;
        }
        D_800B7C18[i].addr &= 0x0FFFFFFF;
        D_800B7C18[i].addr |= 0x40000000;
        D_800B7C18[i].size += D_800B7C18[D_800B7C14].size;
        D_800B7C14 = i;
    }
}
