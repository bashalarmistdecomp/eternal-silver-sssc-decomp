/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libspu/s_m_m.c) and psyz (decomp/src/libspu/s_m_m.c), all
   MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libspu, member s_m_m.o: SpuMalloc. The first split had it in
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
/* far_syms: D_800B7B48 */
/* far_syms: D_800B7B4C */
/* far_syms: D_800B7BD8 */
/* far_syms: D_800B7BE0 */
/* far_syms: D_800B7C10 */
extern s32 D_800B7B48;          /* _spu_rev_reserve_wa: non-zero = the reverb work area is reserved */
extern s32 D_800B7B4C;          /* _spu_rev_offsetaddr: the reverb work area's start, in SPU units */
extern s32 D_800B7BD8;          /* _spu_mem_mode_plus */
extern s32 D_800B7BE0;          /* _spu_mem_mode_unitM: the allocation unit's mask (7) */
extern s32 D_800B7C10;          /* _spu_AllocBlockNum: the table's capacity in records */
void _spu_gcSPU(void);  /* TU defines this below */

/* libspu SpuMalloc(size) (S_M_M.OBJ): allocate size bytes of SPU RAM from the table
   SpuInitMalloc started; returns the block's SPU address, or -1.
   size: if it has a bit above the unit mask (size & ~_spu_mem_mode_unitM, i.e. size >= 8)
   the mask is added, then the low _spu_mem_mode_plus bits are cleared: a size of 8 or more
   is rounded up to a multiple of 8, a size below 8 becomes 0.
   The search: if record 0 is the unallocated tail (addr & 0x40000000) it is taken;
   otherwise the table is collected (_spu_gcSPU) and the first of the _spu_AllocBlockNum
   records that is the tail, or a free block (addr & 0x80000000) of at least size bytes,
   is taken. No such record: -1.
   Taking the tail: its index must be below _spu_AllocBlockNum and its size, less the
   reserved reverb work area ((0x10000 - _spu_rev_offsetaddr) << _spu_mem_mode_plus bytes
   when _spu_rev_reserve_wa is set), must be at least size; else -1. The next record
   becomes the tail (address + size, size less), the found one an allocated block of size
   bytes (flags cleared), and _spu_AllocLastNum the new tail's index.
   Taking a free block: if it is larger than size and _spu_AllocLastNum is below
   _spu_AllocBlockNum, the remainder becomes a free record at index _spu_AllocLastNum, the
   record that was there (the tail) moves up one and _spu_AllocLastNum is incremented. In
   every case the found record's size becomes size and its flags are cleared; without a
   spare record the remainder is recorded nowhere.
   Both arms call _spu_gcSPU again and return the found index's address as read back.
   MARKED SPELLING: the `*(volatile int *)` cast on the found record's address in the tail
   arm is psyz's and sotn-decomp's spelling of a second read the target makes with no store
   in between; a plain read scores 157/177, and the original construct is not known. */
/* func_8009EA48 */
long SpuMalloc(long size) {
    s32 found;
    s32 reserved;
    s32 i;

    i = 0;
    found = -1;
    if (D_800B7B48 == 0) {
        reserved = 0;
    } else {
        reserved = (0x10000 - D_800B7B4C) << D_800B7BD8;
    }
    size += (size & ~D_800B7BE0) ? D_800B7BE0 : 0;
    size >>= D_800B7BD8;
    size <<= D_800B7BD8;
    if (D_800B7C18[0].addr & 0x40000000) {
        found = 0;
    } else {
        _spu_gcSPU();
        for (; i < D_800B7C10; i++) {
            if (D_800B7C18[i].addr & 0x40000000 ||
                (D_800B7C18[i].addr & 0x80000000 && D_800B7C18[i].size >= size)) {
                found = i;
                break;
            }
        }
    }
    if (found == -1) {
        return -1;
    }
    if (D_800B7C18[found].addr & 0x40000000) {
        if (found < D_800B7C10 && D_800B7C18[found].size - reserved >= size) {
            s32 next = found + 1;

            D_800B7C18[next].addr =
                (*(volatile int *) &D_800B7C18[found].addr & 0x0FFFFFFF) + size | 0x40000000;
            D_800B7C18[next].size = D_800B7C18[found].size - size;
            D_800B7C18[found].addr &= 0x0FFFFFFF;
            D_800B7C18[found].size = size;
            D_800B7C14 = next;
            _spu_gcSPU();
            return D_800B7C18[found].addr;
        }
    } else {
        if (size < D_800B7C18[found].size && D_800B7C14 < D_800B7C10) {
            u32 addr = D_800B7C18[found].addr + size;
            u32 rest = D_800B7C18[found].size - size;
            u32 swapAddr = D_800B7C18[D_800B7C14].addr;
            u32 swapSize = D_800B7C18[D_800B7C14].size;

            D_800B7C18[D_800B7C14].addr = addr | 0x80000000;
            D_800B7C18[D_800B7C14].size = rest;
            D_800B7C18[D_800B7C14 + 1].addr = swapAddr;
            D_800B7C18[D_800B7C14 + 1].size = swapSize;
            D_800B7C14++;
        }
        D_800B7C18[found].size = size;
        D_800B7C18[found].addr &= 0x0FFFFFFF;
        _spu_gcSPU();
        return D_800B7C18[found].addr;
    }
    return -1;
}

INCLUDE_PAD(12);
