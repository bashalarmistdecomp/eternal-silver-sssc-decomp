/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libetc/intr_dma.c) and psy-q-decomp (src/etc/intr_dma.c),
   all MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libetc, member intr_dma.o, part 2 of 2: func_800954D4,
   func_80095654, func_80095700. The first split had it in psyq_spu_k.c. The
   tree compiles this run of the member's functions with other flags than
   the runs beside it (config/cc_flags.txt), so the member is in several
   files.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
typedef void (*Callback)();     /* libetc INTR_DMA.OBJ: a DMA channel's completion callback */
/* cc97_lib: func_800954D4 */
/* return_slot_fill: func_800954D4 */
/* far_syms: D_800B7980 */
/* far_syms: D_800B79A4 */
extern volatile u_long *D_800B7980;     /* INTR_DMA.OBJ .data+0: -> DICR, the DMA interrupt register (0x1F8010F4) */
extern Callback D_800B7984[8];          /* INTR_DMA.OBJ .data+4: the DMA callback slots, one per channel */
extern u_long *D_800B79A4;              /* INTR_DMA.OBJ .data+0x24: -> the DMA channel registers (0x1F801080) */
extern char D_80013814[];               /* "DMA bus error: code=%08x\n" */
extern char D_80013830[];               /* "MADR[%d]=%08x\n" */
s32 printf();

/* libetc trapIntrDMA (INTR_DMA.OBJ, static): the handler startIntrDMA
    installs on interrupt 3 (DMA).  While DICR's flag
   bits 24-30 show a finished channel: for each channel i of 0..6 whose flag
   is set, acknowledge it (write DICR back with the low 24 bits kept and of
   the flags only bit 24 + i set, which clears that flag) and call the
   channel's callback (D_800B7984[i]) when one is installed; then read the
   flags again.  Afterwards, when the top byte of DICR is exactly 0x80 (the
   master flag with no channel flag) or bit 15 (the bus-error bit) is set,
   print "DMA bus error: code=%08x" with DICR and the seven channels' MADR
   registers. */
void func_800954D4(void)
{
    u32 mask;
    int i;

    while ((mask = (*D_800B7980 >> 24) & 0x7f) != 0) {
        for (i = 0; mask != 0 && i < 7; i++, mask >>= 1) {
            if (mask & 1) {
                *D_800B7980 &= (0xffffff | (1 << (i + 24)));
                if (D_800B7984[i] != 0) {
                    D_800B7984[i]();
                }
            }
        }
    }

    if ((*D_800B7980 & 0xFF000000) == 0x80000000 || *D_800B7980 & 0x8000) {
        printf(D_80013814, *D_800B7980);
        for (i = 0; i < 7; i++) {
            printf(D_80013830, i, D_800B79A4[4 * i]);
        }
    }
}
/* cc97_lib: func_80095654 */

/* libetc setIntrDMA (INTR_DMA.OBJ, static): install `callback` as DMA
   channel `index`'s completion callback and return the one it replaces
   (nothing is written when it is the same).  A non-null callback is stored
   and the channel's interrupt enabled: DICR = low 24 bits kept (so no
   pending flag in bits 24-30 is acknowledged by the write), master enable
   (bit 23) set, enable bit 16 + index set.  A null callback clears the slot
   and the channel's enable bit, the master enable staying set. */
Callback func_80095654(int index, Callback callback)
{
    Callback prev = D_800B7984[index];

    if (callback != prev) {
        if (callback != 0) {
            D_800B7984[index] = callback;
            *D_800B7980 = (*D_800B7980 & 0xFFFFFF) | 0x800000 | (1 << (index + 16));
        } else {
            D_800B7984[index] = 0;
            *D_800B7980 = ((*D_800B7980 & 0xFFFFFF) | 0x800000) & ~(1 << (index + 16));
        }
    }
    return prev;
}

/* cc97_lib: func_80095700 */

/* libetc memclr (INTR_DMA.OBJ, static): clear `size` words from `ptr`.
   startIntrDMA clears the eight DMA callback slots
   (D_800B7984) with it. */
void func_80095700(s32 *ptr, s32 size)
{
    s32 i;

    for (i = size - 1; i != -1; i--) {
        *ptr++ = 0;
    }
}

INCLUDE_PAD(4);
