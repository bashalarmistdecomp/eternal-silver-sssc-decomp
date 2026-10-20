/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libspu/s_m_f.c) and psyz (decomp/src/libspu/s_m_f.c), all
   MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libspu, member s_m_f.o: SpuFree. The first split had it in
   psyq_misc_ac.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: SpuFree */
/* far_syms: D_800B7C10 */
/* far_syms: D_800B7C18 */
extern s32 D_800B7C10;          /* _spu_AllocBlockNum: the table's capacity in records */
extern SPU_MALLOC *D_800B7C18;  /* _spu_memList */
void _spu_gcSPU(void);

/* libspu SpuFree(addr) (S_M_F.OBJ): give a block SpuMalloc returned back to the table.
   The first _spu_AllocBlockNum records are searched, up to the unallocated tail (addr &
   0x40000000), for the one whose address word equals addr; it gets the free flag
   0x80000000. Found or not, the table is then collected (_spu_gcSPU), which merges
   neighbouring free blocks and folds a free block in front of the tail into it. Caller:
   SsVabClose (libsnd_vs_vab.c), with a VAB's sample area _svm_vab_start[vabId]. */
/* func_8009F018 */
void SpuFree(unsigned long addr) {
    s32 i;

    for (i = 0; i < D_800B7C10; i++) {
        if (D_800B7C18[i].addr & 0x40000000) {
            break;
        }
        if (D_800B7C18[i].addr == addr) {
            D_800B7C18[i].addr |= 0x80000000;
            break;
        }
    }
    _spu_gcSPU();
}

INCLUDE_PAD(4);
