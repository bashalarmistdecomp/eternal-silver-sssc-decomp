/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libspu/s_m_init.c) and psyz (decomp/src/libspu/s_m_init.c),
   all MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libspu, member s_m_init.o: SpuInitMalloc. The first split had
   it in psyq_misc_aa.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: _SpuDataCallback */
     
/* cc97_lib: SpuInitMalloc */
/* far_syms: D_800B7BD8 */
/* far_syms: D_800B7C10 */
/* far_syms: D_800B7C14 */
/* far_syms: D_800B7C18 */
extern s32 D_800B7BD8;          /* _spu_mem_mode_plus */
extern s32 D_800B7C10;          /* _spu_AllocBlockNum: the table's capacity in records */
extern s32 D_800B7C14;          /* _spu_AllocLastNum: index of the last record */
extern SPU_MALLOC *D_800B7C18;  /* _spu_memList */

/* libspu SpuInitMalloc(num, top) (S_M_INIT.OBJ): start the SPU-RAM allocation table in the
   caller's buffer `top`, which must hold num + 1 records. Record 0 becomes the unallocated
   tail (0x40000000) at SPU address 0x1010, with all of SPU RAM above it as its size:
   (0x10000 << _spu_mem_mode_plus) - 0x1010. Returns num, or 0 (and touches nothing) when
   num <= 0. */
/* func_8009E9E8 */
long SpuInitMalloc(long num, char *top) {
    if (num > 0) {
        D_800B7C18 = (SPU_MALLOC *) top;
        D_800B7C18[0].addr = 0x40001010;
        D_800B7C18[0].size = (0x10000 << D_800B7BD8) - 0x1010;
        D_800B7C14 = 0;
        D_800B7C10 = num;
        return num;
    }
    return 0;
}
INCLUDE_PAD(12);
