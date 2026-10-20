/* SPDX-License-Identifier: MIT
   Parts of this file match code of psyz (decomp/src/libpress/libpress.c) and
   psy-q-decomp (src/press/libpress.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* Lunar SSSC MENU.BIN overlay — libpress_libpress: LIBPRESS.OBJ of Sony's libpress,
   release 4.1 (0x80196BDC-0x8019731C), the release whose bytes it is. Its
   ten public functions, DecDCTReset to DecDCToutCallback, carry Sony's
   names; the static MDEC helpers after them (reset, in, out, the two syncs,
   status, the timeout report), which the library's symbol table does not
   name, keep their addresses.

   Compiled -G0 (cc_flags.txt): the overlay never uses $gp. Every function
   is built with the PSY-Q library recipe, `cc272_lib` (GCC 2.7.2 with its
   own delayed branches, then ASPSX's fill of the slots it left empty) —
   the byte evidence is DecDCTin, whose epilogue 2.8.1 cannot produce.
   Two per-function assembler passes complete it, each where the bytes
   show it: `return_slot_fill` where the instruction before `jr $ra` sits
   in its slot (ASPSX moves it unless that would leave `lw $ra` right
   before the `jr`: DecDCTin, the syncs and the callbacks keep the nop),
   and `reorder_fill` where an `la` before a `jal` is split across the
   slot (`lui $a0 / jal / addiu $a0`). */

#include "include_asm.h"
#include "psyq_types.h"
#include <libpress.h>

typedef void (*Callback)();

Callback DMACallback(s32 dma, Callback f);        /* libetc DMACallback */
s32 printf();                                /* printf */

extern char D_8019605C[];   /* "MDEC_rest:bad option(%d)\n" */
extern char D_80196078[];   /* "MDEC_in_sync" */
extern char D_80196088[];   /* "MDEC_out_sync" */
extern char D_80196098[];   /* "\t DMA=(%d,%d), ADDR=(0x%08x->0x%08x)\n" */
extern char D_801960C0[];   /* "\t FIFO=(%d,%d),BUSY=%d,DREQ=(%d,%d),RGB24=%d,STP=%d\n" */
extern char D_801960F8[];   /* "%s timeout:\n" */

/* The default tables as two MDEC command packets: 0x40000001 (set the
   quantization tables) + iq_y[64] + iq_c[64], and 0x60000000 (set the
   IDCT table) + dct[64]. DecDCTGetEnv / DecDCTPutEnv read and write the
   tables, MDEC_reset(0) and DecDCTPutEnv send both packets. */
extern u_long D_80197734[];
extern u_long D_80197738[];
extern u_long D_80197778[];
extern u_long D_801977B8[];
extern u_long D_801977BC[];

/* Hardware register pointers, initialised data in the object. */
extern volatile u_long *D_80197844;   /* DMA0 (MDEC in) MADR  0x1F801080 */
extern volatile u_long *D_80197848;   /* DMA0 BCR             0x1F801084 */
extern volatile u_long *D_8019784C;   /* DMA0 CHCR            0x1F801088 */
extern volatile u_long *D_80197850;   /* DMA1 (MDEC out) MADR 0x1F801090 */
extern volatile u_long *D_80197854;   /* DMA1 BCR             0x1F801094 */
extern volatile u_long *D_80197858;   /* DMA1 CHCR            0x1F801098 */
extern volatile u_long *D_80197874;   /* MDEC0 command/data   0x1F801820 */
extern volatile u_long *D_80197878;   /* MDEC1 control/status 0x1F801824 */
extern volatile u_long *D_8019787C;   /* DPCR                 0x1F8010F0 */

void MDEC_rest(int option);
void func_80196F8C(u_long *adr, int size);
void func_8019701C(u_long *adr, int size);
int MDEC_in_sync(void);
int MDEC_out_sync(void);
u_long func_801971D0(void);
int func_801971E8(char *s);

/* DecDCTReset(mode): mode 0 also resets the callbacks (ResetCallback),
   then MDEC_reset(mode). */
/* cc272_lib: DecDCTReset */
/* return_slot_fill: DecDCTReset */
/* func_80196BDC */
void DecDCTReset(int mode)
{
    if (mode == 0) {
        ResetCallback();
    }
    MDEC_rest(mode);
}

/* DecDCTGetEnv(env): copies the current tables into *env. */
/* cc272_lib: DecDCTGetEnv */
/* func_80196C10 */
DECDCTENV *DecDCTGetEnv(DECDCTENV *env)
{
    u_long *s, *d;
    int i;

    d = (u_long *)env->iq_y;
    s = D_80197738;
    for (i = 15; i != -1; i--) {
        *d++ = *s++;
    }
    d = (u_long *)env->iq_c;
    s = D_80197778;
    for (i = 15; i != -1; i--) {
        *d++ = *s++;
    }
    d = (u_long *)env->dct;
    s = D_801977BC;
    for (i = 31; i != -1; i--) {
        *d++ = *s++;
    }
    return env;
}

/* DecDCTPutEnv(env): takes env's quantization tables (not its dct, which
   stays the default) and sends both packets to the MDEC. */
/* cc272_lib: DecDCTPutEnv */
/* return_slot_fill: DecDCTPutEnv */
/* func_80196C9C */
DECDCTENV *DecDCTPutEnv(DECDCTENV *env)
{
    u_long *s, *d;
    int i;

    d = D_80197738;
    s = (u_long *)env->iq_y;
    for (i = 15; i != -1; i--) {
        *d++ = *s++;
    }
    d = D_80197778;
    s = (u_long *)env->iq_c;
    for (i = 15; i != -1; i--) {
        *d++ = *s++;
    }
    func_80196F8C(D_80197734, 32);
    func_80196F8C(D_801977B8, 32);
    return env;
}

/* DecDCTBufSize(bs): the run-level buffer's size, its first halfword. */
/* cc272_lib: DecDCTBufSize */
/* func_80196D34 */
int DecDCTBufSize(u_long *bs)
{
    return *bs & 0xFFFF;
}

/* DecDCTin(buf, mode): sets the output depth (bit 27 clear = 24-bit when
   mode bit 0) and the STP bit (bit 25, mode bit 1) in the command word,
   then MDEC_in(buf, its low halfword). */
/* cc272_lib: DecDCTin */
/* func_80196D40 */
void DecDCTin(u_long *buf, int mode)
{
    if (mode & 1) {
        *buf &= 0xF7FFFFFF;
    } else {
        *buf |= 0x08000000;
    }
    if (mode & 2) {
        *buf |= 0x02000000;
    } else {
        *buf &= 0xFDFFFFFF;
    }
    func_80196F8C(buf, *buf & 0xFFFF);
}

/* DecDCTout(buf, size): MDEC_out(buf, size). */
/* cc272_lib: DecDCTout */
/* func_80196DBC */
void DecDCTout(u_long *buf, int size)
{
    func_8019701C(buf, size);
}

/* DecDCTinSync(mode): mode 0 waits (MDEC_in_sync); otherwise the MDEC's
   command-busy bit (status bit 29). */
/* cc272_lib: DecDCTinSync */
/* func_80196DDC */
int DecDCTinSync(int mode)
{
    int ret;

    if (mode == 0) {
        ret = MDEC_in_sync();
    } else {
        ret = (func_801971D0() >> 29) & 1;
    }
    return ret;
}

/* DecDCToutSync(mode): mode 0 waits (MDEC_out_sync); otherwise status
   bit 24. */
/* cc272_lib: DecDCToutSync */
/* func_80196E18 */
int DecDCToutSync(int mode)
{
    int ret;

    if (mode == 0) {
        ret = MDEC_out_sync();
    } else {
        ret = (func_801971D0() >> 24) & 1;
    }
    return ret;
}

/* DecDCTinCallback(func): DMACallback(0, func), the MDEC-in channel. */
/* cc272_lib: DecDCTinCallback */
/* func_80196E54 */
int DecDCTinCallback(void (*func)())
{
    return (int)DMACallback(0, func);
}

/* DecDCToutCallback(func): DMACallback(1, func), the MDEC-out channel. */
/* cc272_lib: DecDCToutCallback */
/* func_80196E78 */
int DecDCToutCallback(void (*func)())
{
    return (int)DMACallback(1, func);
}

/* MDEC_reset(option): 0 = reset the MDEC, stop both DMA channels and
   resend the default tables; 1 = the same without the tables (the read of
   DMA1 CHCR is in the bytes); anything else prints
   "MDEC_rest:bad option(%d)". */
/* cc272_lib: MDEC_rest */
/* reorder_fill: MDEC_rest */
/* func_80196E9C */
void MDEC_rest(int option)
{
    switch (option) {
    case 0:
        *D_80197878 = 0x80000000;
        *D_8019784C = 0;
        *D_80197858 = 0;
        *D_80197878 = 0x60000000;
        func_80196F8C(D_80197734, 32);
        func_80196F8C(D_801977B8, 32);
        break;
    case 1:
        *D_80197878 = 0x80000000;
        *D_8019784C = 0;
        *D_80197858 = 0;
        *D_80197858;
        *D_80197878 = 0x60000000;
        break;
    default:
        printf(D_8019605C, option);
        break;
    }
}

/* MDEC_in(adr, size): waits for the previous input, enables DMA0 and
   DMA1 in DPCR, writes the packet's first word (the command) to MDEC0
   and DMAs the rest, size words in 32-word blocks. */
/* cc272_lib: func_80196F8C */
/* return_slot_fill: func_80196F8C */
void func_80196F8C(u_long *adr, int size)
{
    MDEC_in_sync();
    *D_8019787C |= 0x88;
    *D_80197844 = (u_long)(adr + 1);
    *D_80197848 = (((u_long)size >> 5) << 16) | 0x20;
    *D_80197874 = *adr;
    *D_8019784C = 0x01000201;
}

/* MDEC_out(adr, size): waits for the previous output, then starts DMA1
   from the MDEC into adr, size words in 32-word blocks. */
/* cc272_lib: func_8019701C */
/* return_slot_fill: func_8019701C */
void func_8019701C(u_long *adr, int size)
{
    MDEC_out_sync();
    *D_8019787C |= 0x88;
    *D_80197858 = 0;
    *D_80197850 = (u_long)adr;
    *D_80197854 = (((u_long)size >> 5) << 16) | 0x20;
    *D_80197858 = 0x01000200;
}

/* MDEC_in_sync(): waits while the MDEC is busy (status bit 29), at most
   0x100000 polls; on timeout reports "MDEC_in_sync" and returns -1. */
/* cc272_lib: MDEC_in_sync */
/* reorder_fill: MDEC_in_sync */
/* func_801970A8 */
int MDEC_in_sync(void)
{
    volatile int cnt = 0x100000;

    while (*D_80197878 & 0x20000000) {
        if (--cnt == -1) {
            func_801971E8(D_80196078);
            return -1;
        }
    }
    return 0;
}

/* MDEC_out_sync(): waits while DMA1 is busy (CHCR bit 24), at most
   0x100000 polls; on timeout reports "MDEC_out_sync" and returns -1. */
/* cc272_lib: MDEC_out_sync */
/* reorder_fill: MDEC_out_sync */
/* func_8019713C */
int MDEC_out_sync(void)
{
    volatile int cnt = 0x100000;

    while (*D_80197858 & 0x01000000) {
        if (--cnt == -1) {
            func_801971E8(D_80196088);
            return -1;
        }
    }
    return 0;
}

/* MDEC_status(): the MDEC1 status word. */
/* cc272_lib: func_801971D0 */
u_long func_801971D0(void)
{
    return *D_80197878;
}

/* timeout(s): prints "<s> timeout:", both DMA channels' busy bits and
   addresses, and the status word's FIFO / BUSY / DREQ / RGB24 / STP bits,
   then resets the MDEC as MDEC_reset(1) does. */
/* cc272_lib: func_801971E8 */
/* return_slot_fill: func_801971E8 */
int func_801971E8(char *s)
{
    u_long st;

    printf(D_801960F8, s);
    st = *D_80197878;
    printf(D_80196098, (*D_8019784C >> 24) & 1, (*D_80197858 >> 24) & 1,
                  *D_80197844, *D_80197850);
    printf(D_801960C0, ~st >> 31, (st >> 30) & 1, (st >> 29) & 1,
                  (st >> 28) & 1, (st >> 27) & 1, (st >> 25) & 1, (st >> 23) & 1);
    *D_80197878 = 0x80000000;
    *D_8019784C = 0;
    *D_80197858 = 0;
    *D_80197858;
    *D_80197878 = 0x60000000;
    return 0;
}

/* The object's .text ends 8 zero bytes after timeout's last instruction
   (0x80197314): its size is 0x740, a multiple of 16, and the vlc object
   follows at 0x8019731C. */
INCLUDE_PAD(8);
