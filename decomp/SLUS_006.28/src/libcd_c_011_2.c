/* PSY-Q 4.5 libcd, member c_011.o, part 2 of 2: func_80094670. The first
   split had it in psyq_spu_e.c. The tree compiles this run of the member's
   functions with other flags than the runs beside it (config/cc_flags.txt),
   so the member is in several files.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: func_80094670 */
/* gnu_as_loads: func_80094670 */
/* return_slot_fill: func_80094670 */
/* reorder_fill: func_80094670 */
/* far_syms: D_800B67E0 (target reaches it %hi/%lo) */
/* far_syms: D_800B67F8 (target reaches it %hi/%lo) */
/* far_syms: D_800B67FC (target reaches it %hi/%lo) */
extern volatile u8 *D_800B67E0;
extern volatile u32 *D_800B67F8;
extern volatile u32 *D_800B67FC;
extern char D_80013774[];
s32 printf();

/* libcd dma_execute (C_011.OBJ, the static after mem2mem): start one DMA transfer on
   channel ch (StCdInterrupt calls it with 3, the CD-ROM channel).  Waits while the
   channel's CHCR (0x1F801088 + ch * 16) has its busy bit (24) set, at most 0x10000
   polls; on the last one it prints "DMA STATUS ERROR %x" (D_80013774) with CHCR through
   printf and goes on.  mode == 1 sets, anything else clears, bit ch of
   byte 2 of DICR (through D_800B67FC -> 0x1F8010F4: the channel's interrupt enable),
   and DICR is read back into a volatile local either way.  Then DPCR (through
   D_800B67F8 -> 0x1F8010F0) |= 1 << (ch * 4 + 3) (the channel's master enable), MADR =
   madr and BCR = (blocks << 16) | size are written through a pointer that walks the
   channel's three registers (0x1F801080 + ch * 16), the CD-ROM index/status register
   (through D_800B67E0 -> 0x1F801800) is polled until its bit 6 (data FIFO not empty)
   is set, CHCR = chcr starts the transfer and CHCR is read back.  The seventh argument
   every caller passes is not read.
   MARKED SPELLING: no reference source exists for this static, so three shapes here are
   inferred from the bytes and are semantically inert against the plain forms: the DICR
   byte through the local `b` in both arms (`p[2] |= 1 << ch` directly: 89/106), the
   `dummy = *D_800B67FC` read written in both arms (once after the if / else: 98/106),
   and the DPCR update as single steps on `n` and `m` (`*D_800B67F8 |= 1 << (ch * 4 +
   3)`: 85/106).  The original text is not known. */
void func_80094670(s32 ch, u32 *madr, s32 blocks, s32 size, u32 chcr, u8 mode, s32 unused)
{
    volatile u32 dummy;
    s32 i;
    volatile u32 *p;
    u8 b;
    u32 n;
    u32 m;

    i = 0;
    while (*(volatile u32 *)(0x1F801088 + (ch << 4)) & 0x01000000) {
        if (i == 0x10000) {
            printf(D_80013774, *(volatile u32 *)(0x1F801088 + (ch << 4)));
            break;
        }
        i++;
    }
    if (mode == 1) {
        b = ((volatile u8 *)D_800B67FC)[2];
        b |= 1 << ch;
        ((volatile u8 *)D_800B67FC)[2] = b;
        dummy = *D_800B67FC;
    } else {
        b = ((volatile u8 *)D_800B67FC)[2];
        b &= ~(1 << ch);
        ((volatile u8 *)D_800B67FC)[2] = b;
        dummy = *D_800B67FC;
    }
    n = ch << 2;
    n += 3;
    m = 1;
    m <<= n;
    n = *D_800B67F8;
    n |= m;
    *D_800B67F8 = n;
    p = (volatile u32 *)((ch << 4) + 0x1F801080);
    *p++ = (u32)madr;
    *p++ = (blocks << 16) | size;
    while (!(*D_800B67E0 & 0x40)) {
    }
    *p = chcr;
    dummy = *p;
}
