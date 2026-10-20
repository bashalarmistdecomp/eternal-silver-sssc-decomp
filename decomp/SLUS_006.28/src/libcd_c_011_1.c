/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp (src/main/psxsdk/libcd/c_011.c)
   and psyz (decomp/src/libcd/c_011.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libcd, member c_011.o, part 1 of 2: StCdInterrupt,
   func_80094644. The first split had it in psyq_spu_d.c. The tree compiles
   this run of the member's functions with other flags than the runs beside
   it (config/cc_flags.txt), so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: StCdInterrupt */
/* far_syms: D_800B67E0 */
/* far_syms: D_800B67E8 */
/* far_syms: D_800B67EC */
/* far_syms: D_800B67F0 */
/* far_syms: D_800B67F4 */
/* far_syms: D_800B6800 */
/* far_syms: D_800B6810 */
/* far_syms: D_800B6828 */
s32 CdReady(s32, u8 *);
void data_ready_callback();
void init_ring_status();
void func_80094644();  /* TU defines this below */
void func_80094670();
extern volatile u8 *D_800B67E0;
extern volatile u8 *D_800B67E8;
extern volatile u8 *D_800B67EC;
extern volatile u32 *D_800B67F0;
extern volatile u32 *D_800B67F4;
extern volatile u32 *D_800B6800;
extern volatile u32 *D_800B6810;
extern s32 D_800B6828;
extern u32 D_800E70C0;
extern s16 D_800E70C4;
extern s32 D_800E70C8;
extern s32 D_800E70CC;
extern s32 D_800E70D0;
extern u32 D_800E70D4;
extern s32 D_800E70D8;
extern s32 D_800E70DC;
extern s32 D_800E70E0;
extern s32 D_800E70E4;
extern s32 D_800E70E8;
extern u32 *D_800E70F0;
extern u32 D_800E70F4;
extern s32 D_800E70F8;
extern u32 *D_800E70FC;
extern u32 *D_800E7100;
extern s32 D_800E7104;
extern s32 D_800E7138;
extern void (*D_800E9574)();
extern volatile u16 *D_800E9578;       /* C_011.OBJ .bss+0: the arriving sector's ring header, as u16 words */

/* libcd StCdInterrupt (C_011.OBJ): the streaming library's CD data-ready handler; it
   takes one arriving sector into the ring.  The ring (StRingAddr, D_800E7100) is
   StRingSize (D_800E7104) 8-word headers followed by the 504-word sector bodies.
   D_800E9578 views the sector's header as u16 words (libcd.h's indices: 0 status, 1
   type, 2 sector offset, 3 sector size, 4 frame number, low half).  The numbered exits
   leave their number in debug_cause (D_800B6828).
   - Nothing is done while StFinalSector (D_800E70DC) is 1.
   - StRgb24 (D_800E70C8) set and bit 24 of DMA channel 1's CHCR (0x1F801098, through
     D_800B6800) set: StCdIntrFlag (D_800E70CC) = 1, exit 1.
   - CdReady(1, result) == 5 (CdlDiskError): return.  result[0] & 4: exit 3.
   - The header is slot StRingIdx1 (D_800E70E4).  Its status not 0 (StFREE): exit 4.
   - The CD registers are written (0x1F801800 = 0, 0x1F801803 = 0, 0x1F801800 = 0,
     0x1F801803 = 0x80), then 0x1F801018 = 0x20943 and 0x1F801020 = 0x1323.  When
     StMode (D_800E7138) is 0, four bytes are read from the data register (0x1F801802)
     into `loc` and eight more are read and dropped.
   - The 8 header words come in: mem2mem (func_80094644) from the emulation buffer
     (StEmu_Addr, D_800E70F0, sector StEmu_Idx, D_800E70E0) when one is set, else
     dma_execute (func_80094670) on channel 3; then the wait for bit 24 of DMA channel
     3's CHCR (0x1F8010B8, through D_800B6810) to clear.  `loc` goes into the header's
     loc; 0x1F801018 = 0x20843, 0x1F801020 = 0x1325.
   - StSTART_FLAG (D_800E70F8) 1 with a StStartFrame (D_800E70D4): a sector of another
     frame is dropped (status 0, return); the start frame's clears the flag.
   - Status not 0x160, or channel ((type >> 10) & 0x1F) not CChannel (D_800E70D8): exit
     5, status 0 (without emulation the status is read once more and dropped first).
   - Sector offset not Stsector_offset (D_800E70C4), or Stframe_no (D_800E70C0) set and
     not this frame: both cleared, init_ring_status called with
     StRingIdx2 (D_800E70E8) and the count StRingIdx1 - StRingIdx2, StRingIdx1 =
     StRingIdx2, status 0, exit 6.
   - A frame's first sector (offset 0) clears Stsector_offset and sets Stframe_no.  At
     or past StEndFrame (D_800E70F4): the same reset, StSTART_FLAG = 1, StFunc2
     (D_800E9574) called if set, exit 7.  When StRingSize - StRingIdx1 - 1 is below the
     frame's sector count: without an end frame status 1, StSTART_FLAG = 1, StFunc2,
     exit 8; with slot 0's status not 0, status 0 and exit 9; else status 1 (StREWIND),
     the header is copied to slot 0, StRingIdx1 = 0 and D_800E9578 moves there.  Then
     StRingIdx2 = StRingIdx1.
   - Exit code 10: Stsector_offset++, StRingBase (D_800E70FC) = the body of slot
     StRingIdx1, and the body's 504 words come in the same two ways (with StRgb24
     0x1F801018 = 0x20943, 0x1F801020 = 0x1323, CHCR 0x11000000; else 0x1F801018 =
     0x21020843, CHCR 0x11400100).  The frame's last sector (offset == count - 1) sets
     StFinalSector, passes 1 as the transfer's mode, clears Stsector_offset and
     Stframe_no and copies StCHANNEL (D_800E70D0) to CChannel.
   - 0x1F801020 = 0x1325, status 3 (StBUSY), StRingIdx1++; under emulation with
     StFinalSector set, data_ready_callback is called directly.
   Under emulation exits 1, 4, 6, 7, 8 and 9, the start-frame drop and the body transfer
   advance StEmu_Idx by one; exit 5 zeroes it.
   MARKED SPELLING: `*(s16 *)D_800E7100`, slot 0's status read as a signed short (the
   target has `lh` there and `lhu` at every read through D_800E9578); as `(u16 *)`
   582/583; the original construct is not known. */
/* func_80093D28 */
void StCdInterrupt(void)
{
    volatile u16 st[4];
    CdlLOC loc;
    u8 result[8];
    u32 i;
    u32 chcr;
    u32 *src;
    u32 *dst;

    if (D_800E70DC == 1)
        return;
    if (D_800E70C8 != 0 && (*D_800B6800 & 0x01000000)) {
        D_800E70CC = 1;
        if (D_800E70F0 != 0)
            D_800E70E0++;
        D_800B6828 = 1;
        return;
    }
    if (CdReady(1, result) == 5)
        return;
    st[1] = result[0];
    st[2] = result[1];
    if (st[1] & 4) {
        D_800B6828 = 3;
        return;
    }
    D_800E9578 = (u16 *)(D_800E7100 + D_800E70E4 * 8);
    if (D_800E9578[0] != 0) {
        if (D_800E70F0 != 0)
            D_800E70E0++;
        D_800B6828 = 4;
        return;
    }
    *D_800B67E0 = 0;
    *D_800B67EC = 0;
    *D_800B67E0 = 0;
    *D_800B67EC = 0x80;
    *D_800B67F0 = 0x20943;
    *D_800B67F4 = 0x1323;
    if (D_800E7138 == 0) {
        for (i = 0; i < 4; i++)
            ((u8 *)&loc)[i] = *D_800B67E8;
        for (i = 0; i < 8; i++)
            *D_800B67E8;
    }
    chcr = 0x11000000;
    if (D_800E70F0 != 0)
        func_80094644(D_800E9578, D_800E70F0 + D_800E70E0 * 512, 8, 0);
    else
        func_80094670(3, D_800E9578, 0, 8, chcr, 0, 0);
    while (*D_800B6810 & 0x01000000)
        ;
    ((StHEADER *)D_800E9578)->loc = loc;
    *D_800B67F0 = 0x20843;
    *D_800B67F4 = 0x1325;
    if (D_800E70F8 == 1 && D_800E70D4 != 0) {
        if (D_800E70D4 != D_800E9578[4]) {
            D_800E9578[0] = 0;
            if (D_800E70F0 != 0)
                D_800E70E0++;
            return;
        }
        D_800E70F8 = 0;
    }
    if (D_800E9578[0] != 0x160 || ((D_800E9578[1] >> 10) & 0x1F) != D_800E70D8) {
        if (D_800E70F0 != 0)
            D_800E70E0 = 0;
        else
            D_800E9578[0];
        D_800B6828 = 5;
        D_800E9578[0] = 0;
        return;
    }
    if (D_800E70C4 != D_800E9578[2] || (D_800E70C0 != 0 && D_800E70C0 != D_800E9578[4])) {
        D_800E70C0 = 0;
        D_800E70C4 = 0;
        init_ring_status(D_800E70E8, D_800E70E4 - D_800E70E8);
        D_800E70E4 = D_800E70E8;
        D_800E9578[0] = 0;
        if (D_800E70F0 != 0)
            D_800E70E0++;
        D_800B6828 = 6;
        return;
    }
    if (D_800E9578[2] == 0) {
        D_800E70C4 = 0;
        D_800E70C0 = D_800E9578[4];
        if (D_800E70F4 != 0 && D_800E70C0 >= D_800E70F4) {
            D_800E70C0 = 0;
            D_800E70C4 = 0;
            init_ring_status(D_800E70E8, D_800E70E4 - D_800E70E8);
            D_800E70E4 = D_800E70E8;
            D_800E9578[0] = 0;
            D_800E70F8 = 1;
            if (D_800E9574 != 0)
                D_800E9574();
            if (D_800E70F0 != 0)
                D_800E70E0++;
            D_800B6828 = 7;
            return;
        }
        if ((u32)(D_800E7104 - D_800E70E4 - 1) < D_800E9578[3]) {
            if (D_800E70F4 == 0) {
                D_800E9578[0] = 1;
                D_800E70F8 = 1;
                if (D_800E9574 != 0)
                    D_800E9574();
                if (D_800E70F0 != 0)
                    D_800E70E0++;
                D_800B6828 = 8;
                return;
            }
            if (*(s16 *)D_800E7100 != 0) {
                D_800E9578[0] = 0;
                if (D_800E70F0 != 0)
                    D_800E70E0++;
                D_800B6828 = 9;
                return;
            }
            D_800E9578[0] = 1;
            dst = D_800E7100;
            src = (u32 *)D_800E9578;
            D_800E70E4 = 0;
            for (i = 0; i < 8; i++)
                *dst++ = *src++;
            D_800E9578 = (u16 *)D_800E7100;
        }
        D_800E70E8 = D_800E70E4;
    }
    D_800B6828 = 10;
    D_800E70C4++;
    D_800E70FC = D_800E7100 + D_800E7104 * 8 + D_800E70E4 * 504;
    if (D_800E70C8 != 0) {
        chcr = 0x11000000;
        *D_800B67F0 = 0x20943;
        *D_800B67F4 = 0x1323;
    } else {
        *D_800B67F0 = 0x21020843;
        chcr = 0x11400100;
    }
    if (D_800E9578[3] - 1 == D_800E9578[2]) {
        D_800E70DC = 1;
        if (D_800E70F0 != 0) {
            func_80094644(D_800E70FC, D_800E70F0 + D_800E70E0 * 512 + 8, 0x1F8, 1);
            D_800E70E0++;
        } else {
            func_80094670(3, D_800E70FC, 0, 0x1F8, chcr, 1, 0);
        }
        D_800E70C4 = 0;
        D_800E70C0 = 0;
        D_800E70D8 = D_800E70D0;
    } else {
        if (D_800E70F0 != 0) {
            func_80094644(D_800E70FC, D_800E70F0 + D_800E70E0 * 512 + 8, 0x1F8, 0);
            D_800E70E0++;
        } else {
            func_80094670(3, D_800E70FC, 0, 0x1F8, chcr, 0, 0);
        }
    }
    *D_800B67F4 = 0x1325;
    D_800E9578[0] = 3;
    D_800E70E4++;
    if (D_800E70F0 != 0 && D_800E70DC != 0)
        data_ready_callback();
}

/* cc97_lib: func_80094644 */

/* libcd mem2mem (C_011.OBJ, static): copy `num` words from `src` to `dst`.
   StCdInterrupt uses it in place of the DMA when a stream is emulated from
   memory (StEmu_Addr); the fourth argument, the transfer mode it also gives
   dma_execute, is not read. */
void func_80094644(u32 *dst, u32 *src, u32 num, u32 mode)
{
    u32 i;

    for (i = 0; i < num; i++) {
        *dst++ = *src++;
    }
}
