/* SPDX-License-Identifier: MIT
   Parts of this file match code of psyz (decomp/src/libspu/spu.c),
   MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libspu, member spu.o, part 3 of 3: _spu_Fr_, _spu_t, _spu_Fw,
   _spu_Fr, _spu_FsetRXX, _spu_FsetRXXa, _spu_FgetRXXa, _spu_FsetPCR,
   func_8009E900, func_8009E928, _spu_Fw1ts. The first split had it in
   psyq_misc_y.c. The tree compiles this run of the member's functions with
   other flags than the runs beside it (config/cc_flags.txt), so the member
   is in several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
extern u16 D_800B7BC8;              /* _spu_tsa */
extern s32 D_800B7BD8;              /* _spu_mem_mode_plus */
/* cc97_lib: _spu_Fr_ */
/* return_slot_fill: _spu_Fr_ */
/* far_syms: D_800B7BB0 D_800B7BB4 D_800B7BB8 D_800B7BBC D_800B7C00 */
/* libspu's register block as the library declares it (psyz's libspu_private.h, `union
   SpuUnion`): the registers by name, or as 0x100 16-bit words for the functions that are
   handed a register number. SpuRXX is libspu_spu_1.c's. */
extern SpuUnion *D_800B7BB0;        /* _spu_RXX -> 0x1F801C00 */
extern volatile s32 *D_800B7BB4;    /* SPU.OBJ .data+4, static: -> 0x1F8010C0, DMA channel 4 MADR */
extern volatile s32 *D_800B7BB8;    /* SPU.OBJ .data+8, static: -> 0x1F8010C4, DMA channel 4 BCR */
extern volatile s32 *D_800B7BBC;    /* SPU.OBJ .data+0xC, static: -> 0x1F8010C8, DMA channel 4 CHCR */
extern s32 D_800B7C00;              /* SPU.OBJ .data+0x50, static: 1 = the DMA transfer reads */
void _spu_Fw1ts(void);  /* TU defines this below */
void func_8009E928(void);  /* TU defines this below */

/* libspu _spu_Fr_ (SPU.OBJ): start a DMA read of SPU RAM into addr.  tsa is
   the transfer start address as the SPU register takes it, blocks the number
   of 64-byte blocks.  Writes the address register, waits one _spu_Fw1ts,
   sets SPUCNT's transfer mode (bits 4-5) to 3, DMA read, waits again, sets
   the bus delay for reading (func_8009E928) and programs DMA channel 4: MADR
   = addr, BCR = blocks << 16 | 0x10 (16 words a block), the direction flag
   D_800B7C00 = 1, CHCR = 0x01000200 (start, sync by request, to memory).
   No caller in this program. */
/* func_8009E374 */
void _spu_Fr_(s32 addr, u16 tsa, s32 blocks)
{
    D_800B7BB0->rxx.trans_addr = tsa;
    _spu_Fw1ts();
    D_800B7BB0->rxx.spucnt |= 0x30;
    _spu_Fw1ts();
    func_8009E928();
    *D_800B7BB4 = addr;
    *D_800B7BB8 = (blocks << 16) | 0x10;
    D_800B7C00 = 1;
    *D_800B7BBC = 0x01000200;
}
/* cc97_lib: _spu_t */
/* return_slot_fill: _spu_t */
/* far_syms: D_800B7BB4 */
/* far_syms: D_800B7BB8 */
/* far_syms: D_800B7BBC */
/* far_syms: D_800B7BC8 */
/* far_syms: D_800B7C00 */
/* far_syms: D_800B7C04 */
/* far_syms: D_800B7C08 */
extern s32 D_800B7C04;          /* SPU.OBJ .data+0x54 (static): the DMA's memory address */
extern s32 D_800B7C08;          /* SPU.OBJ .data+0x58 (static): the DMA's size in 64-byte blocks */
void func_8009E900(void);  /* TU defines this below */

/* libspu _spu_t(mode, ...) (SPU.OBJ): the steps of an SPU-RAM DMA transfer, one per call.
   mode 2 (address): _spu_tsa = address >> _spu_mem_mode_plus, also written to the SPU's
   transfer-address register (+0x1A6).
   mode 1 (write) / mode 0 (read): record the direction in D_800B7C00 (0 / 1), wait until
   the register reads back _spu_tsa, then set SPUCNT (+0x1AA) bits 4-5 to 2 (DMA write) /
   3 (DMA read).
   mode 3 (buffer, size): wait until SPUCNT's bits 4-5 show that mode, set the SPU bus
   delay for the direction (func_8009E928 reading, func_8009E900 writing), then start DMA
   channel 4: MADR = buffer, BCR = (size rounded up to 64-byte blocks) << 16 | 0x10, CHCR =
   0x01000200 (read) or 0x01000201 (write).
   Each wait gives up after 0xF01 polls and returns -2; everything else returns 0, an
   unknown mode included. Callers: _spu_Fw, _spu_Fr and
   SpuClearReverbWorkArea. */
/* func_8009E41C */
int _spu_t(int mode, ...) {
    unsigned chcr;
    unsigned i;
    void *args;
    unsigned arg;
    u16 want;
    u16 cnt;

    args = (char *) &mode + sizeof(int);                            /* va_start(args, mode) */
    switch (mode) {
    case 2:
        args = (char *) args + sizeof(unsigned);                    /* arg = va_arg(args, unsigned) */
        arg = *(unsigned *) ((char *) args - sizeof(unsigned));
        D_800B7BC8 = arg >> D_800B7BD8;
        D_800B7BB0->rxx.trans_addr = D_800B7BC8;
        break;
    case 1:
        D_800B7C00 = 0;
        i = 0;
        while (D_800B7BB0->rxx.trans_addr != D_800B7BC8) {
            if (++i > 0xF00) {
                return -2;
            }
        }
        cnt = D_800B7BB0->rxx.spucnt;
        cnt &= ~0x30;
        cnt |= 0x20;
        D_800B7BB0->rxx.spucnt = cnt;
        break;
    case 0:
        D_800B7C00 = 1;
        i = 0;
        while (D_800B7BB0->rxx.trans_addr != D_800B7BC8) {
            if (++i > 0xF00) {
                return -2;
            }
        }
        cnt = D_800B7BB0->rxx.spucnt;
        cnt &= ~0x30;
        cnt |= 0x30;
        D_800B7BB0->rxx.spucnt = cnt;
        break;
    case 3:
        if (D_800B7C00 == 1) {
            want = 0x30;
        } else {
            want = 0x20;
        }
        i = 0;
        while ((D_800B7BB0->rxx.spucnt & 0x30) != want) {
            if (++i > 0xF00) {
                return -2;
            }
        }
        if (D_800B7C00 == 1) {
            func_8009E928();
        } else {
            func_8009E900();
        }
        args = (char *) args + sizeof(unsigned);                    /* arg = va_arg(args, unsigned) */
        arg = *(unsigned *) ((char *) args - sizeof(unsigned));
        D_800B7C04 = arg;
        args = (char *) args + sizeof(unsigned);                    /* arg = va_arg(args, unsigned) */
        arg = *(unsigned *) ((char *) args - sizeof(unsigned));
        D_800B7C08 = arg / 0x40;
        D_800B7C08 += (arg % 0x40) ? 1 : 0;
        *D_800B7BB4 = D_800B7C04;
        *D_800B7BB8 = (D_800B7C08 << 0x10) | 0x10;
        if (D_800B7C00 == 1) {
            chcr = 0x01000200;
        } else {
            chcr = 0x01000201;
        }
        *D_800B7BBC = chcr;
        break;
    }
    return 0;
}

/* cc97_lib: _spu_Fw */
/* return_slot_fill: _spu_Fw */
/* far_syms: D_800B7BCC */
extern s32 D_800B7BCC;              /* _spu_transMode: 0 = transfer by DMA */
void func_8009E0F8(u8 *, u32);      /* _spu_FwriteByIO (libspu_spu_1.c) */

/* libspu _spu_Fw(addr, size) (SPU.OBJ): write size bytes at addr to SPU RAM at the
   transfer address _spu_tsa. With _spu_transMode 0 the transfer is a DMA, started by
   three calls of _spu_t: the address (2, _spu_tsa << _spu_mem_mode_plus, a byte address
   again), the direction (1, write), the buffer and its size (3). Otherwise the bytes go
   through the data port (_spu_FwriteByIO). Returns size. Caller: SpuWrite
   . */
/* func_8009E69C */
u32 _spu_Fw(u8 *addr, u32 size)
{
    if (D_800B7BCC == 0) {
        _spu_t(2, D_800B7BC8 << D_800B7BD8);
        _spu_t(1);
        _spu_t(3, addr, size);
    } else {
        func_8009E0F8(addr, size);
    }
    return size;
}
/* cc97_lib: _spu_Fr */
/* return_slot_fill: _spu_Fr */

/* libspu _spu_Fr(addr, size) (SPU.OBJ): read size bytes of SPU RAM at the transfer
   address _spu_tsa into addr, always by DMA: _spu_t with the address (2), the direction
   (0, read), the buffer and its size (3). Returns size. No caller in this program. */
/* func_8009E720 */
u32 _spu_Fr(u8 *addr, u32 size)
{
    _spu_t(2, D_800B7BC8 << D_800B7BD8);
    _spu_t(0);
    _spu_t(3, addr, size);
    return size;
}

/* cc97_lib: _spu_FsetRXX */

/* libspu _spu_FsetRXX(offset, value, mode) (SPU.OBJ): write one SPU register. offset is
   the register's word index in the block (0x1F801C00 + offset * 2). mode 0 stores value
   as it is; any other mode stores value >> _spu_mem_mode_plus, a byte address turned
   into the register's address unit. Callers: _SpuInit, SpuSetReverbModeParam and
   SpuSetReverbModeType, all with word 0xD1 (the reverb work area's start) and mode 0. */
/* func_8009E784 */
void _spu_FsetRXX(s32 offset, u32 value, s32 mode) {
    if (mode == 0) {
        D_800B7BB0->raw[offset] = value;
    } else {
        D_800B7BB0->raw[offset] = value >> D_800B7BD8;
    }
}
/* cc97_lib: _spu_FsetRXXa */
/* far_syms: D_800B7BB0 */
/* far_syms: D_800B7BD4 */
/* far_syms: D_800B7BD8 */
/* far_syms: D_800B7BDC */
/* far_syms: D_800B7BE0 */
extern s32 D_800B7BD4;          /* _spu_mem_mode: non-zero = addresses are rounded to the unit */
extern s32 D_800B7BDC;          /* _spu_mem_mode_unit: the allocation unit in bytes (8) */
extern s32 D_800B7BE0;          /* _spu_mem_mode_unitM: the unit's mask (7) */

/* libspu _spu_FsetRXXa(offset, unit) (SPU.OBJ): write a byte address to an SPU address
   register. With _spu_mem_mode set, an address that is not a multiple of _spu_mem_mode_unit
   is first rounded up to the next one. The register value is the address shifted right by
   _spu_mem_mode_plus, as a u16. offset is the register's word index in the SPU block
   (0x1F801C00 + offset * 2); two offsets write nothing: -1 returns the register value, -2
   the rounded address. Every other offset stores the value and returns the rounded
   address. */
/* func_8009E7C8 */
long _spu_FsetRXXa(long offset, unsigned long unit) {
    u16 value;

    if (D_800B7BD4 != 0 && (unit % D_800B7BDC) != 0) {
        unit += D_800B7BDC;
        unit &= ~D_800B7BE0;
    }
    value = unit >> D_800B7BD8;

    switch (offset) {
    case -1:
        return value;
    case -2:
        return unit;
    default:
        D_800B7BB0->raw[offset] = value;
        return unit;
    }
}

/* far_syms: D_800B7BB0 (target reaches it %hi/%lo) */
/* far_syms: D_800B7BD8 (target reaches it %hi/%lo) */
/* far_syms: D_800B7BD8 (auto: target reaches it %hi/%lo — addressing row 3) */

/* cc97_lib: _spu_FgetRXXa */

/* libspu _spu_FgetRXXa(offset, mode) (SPU.OBJ): read one SPU address register. offset is
   the register's word index in the block. mode -1 returns the register word as read;
   any other mode returns it << _spu_mem_mode_plus, the byte address. No caller in this
   program. */
/* func_8009E86C */
s32 _spu_FgetRXXa(s32 offset, s32 mode) {
    u16 value = D_800B7BB0->raw[offset];

    if (mode == -1) {
        return value;
    }
    return value << D_800B7BD8;
}

/* far_syms: D_800B7BC0 (target reaches it %hi/%lo) */
/* cc97_lib: _spu_FsetPCR */
/* return_slot_fill: _spu_FsetPCR */
extern volatile s32 *D_800B7BC0;    /* SPU.OBJ .data+0x10, static: -> 0x1F8010F0, DPCR */

/* libspu _spu_FsetPCR(flag) (SPU.OBJ): set DMA channel 4's priority in DPCR (bits
   16-18): the three bits are cleared, then 3 is written when flag is set, 5 when it is
   not. The channel's enable bit (19) is left alone. No caller in this program. */
/* func_8009E8A8 */
void _spu_FsetPCR(s32 flag)
{
    *D_800B7BC0 &= 0xFFF8FFFF;
    if (flag != 0) {
        *D_800B7BC0 |= 0x30000;
    } else {
        *D_800B7BC0 |= 0x50000;
    }
}

/* far_syms: D_800B7BC4 (target reaches it %hi/%lo) */

extern s32 *D_800B7BC4;

void func_8009E900(void) {
    *D_800B7BC4 = (*D_800B7BC4 & 0xF0FFFFFF) | 0x20000000;
}
void func_8009E928(void) {
    *D_800B7BC4 = (*D_800B7BC4 & 0xF0FFFFFF) | 0x22000000;
}
/* cc97_lib: _spu_Fw1ts */
/* return_slot_fill: _spu_Fw1ts */

/* libspu _spu_Fw1ts() (SPU.OBJ): a short busy wait, used between SPU register writes and
   around transfers. Sixty turns of a loop whose counter and work value are both volatile
   stack variables (the value is 13 multiplied by 13 each turn and never used). */
/* func_8009E950 */
void _spu_Fw1ts(void) {
    volatile int i;
    volatile int t;

    t = 13;
    for (i = 0; i < 60; i++) {
        t *= 13;
    }
}
INCLUDE_PAD(12);
