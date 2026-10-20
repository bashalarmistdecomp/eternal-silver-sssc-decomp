/* PSY-Q 4.5 libspu, member spu.o, part 1 of 3: _spu_init, func_8009E0F8.
   The first split had it in psyq_misc_w.c. The tree compiles this run of
   the member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: _spu_init */
/* return_slot_fill: _spu_init */
/* reorder_fill: _spu_init */
/* far_syms: D_800B7BB0 D_800B7BC0 D_800B7BC8 D_800B7BCC D_800B7BD0 D_800B7BD4 */
/* far_syms: D_800B7BD8 D_800B7BDC D_800B7BE0 D_800B7BE4 D_800B7BE8 D_800B7BEC */
/* libspu's register block as the library declares it (psyz's libspu_private.h, `union
   SpuUnion`): the registers by name, or as 0x100 16-bit words for the functions that are
   handed a register number. SpuRXX is libspu_spu_1.c's. */
extern SpuUnion *D_800B7BB0;        /* _spu_RXX -> 0x1F801C00 */
extern volatile s32 *D_800B7BC0;            /* SPU.OBJ .data+0x10, static: -> 0x1F8010F0, DPCR */
extern u16 D_800B7BC8;                      /* _spu_tsa */
extern s32 D_800B7BCC;                      /* _spu_transMode */
extern s32 D_800B7BD0;                      /* _spu_addrMode */
extern s32 D_800B7BD4;                      /* _spu_mem_mode */
extern s32 D_800B7BD8;                      /* _spu_mem_mode_plus */
extern s32 D_800B7BDC;                      /* _spu_mem_mode_unit */
extern s32 D_800B7BE0;                      /* _spu_mem_mode_unitM */
extern s32 D_800B7BE4;                      /* _spu_inTransfer */
extern volatile SpuTransferCallbackProc D_800B7BE8;    /* _spu_transferCallback */
extern volatile SpuIRQCallbackProc D_800B7BEC;         /* _spu_IRQCallback */
extern u8 D_800B7BF0[];                     /* SPU.OBJ .data+0x40, static: sixteen bytes of 7 */
extern volatile u16 D_800EA8C0[];           /* _spu_RQ[10] */
extern char D_800138D4[];                   /* "SPU:T/O [%s]\n" */
extern char D_800138E4[];                   /* "wait (reset)" */
void _spu_Fw1ts(void);
void func_8009E0F8(u8 *, u32);              /* _spu_FwriteByIO */
s32 printf(const char *, ...);

/* libspu _spu_init (SPU.OBJ): reset the SPU; bHot != 0 keeps the voices.
   Enables DMA channel 4 at priority 3 in DPCR (|= 0xB0000), zeroes the main
   volume and SPUCNT, clears _spu_transMode, _spu_addrMode and _spu_tsa, waits
   one _spu_Fw1ts, zeroes the main volume again and polls SPUSTAT's low eleven
   bits until they read 0 (0xF01 polls, then the "wait (reset)" time-out
   print).  Then the memory unit (_spu_mem_mode 2, _plus 3, _unit 8, _unitM
   7), the transfer control word 4, reverb volume 0, key off for all 24
   voices, reverb-on mask 0 and the ten words of the register queue _spu_RQ.
   Cold only: FM and noise masks and CD / external volume 0, the sixteen
   dummy bytes D_800B7BF0 written to SPU RAM 0x1000 by I/O (_spu_tsa = 0x200,
   _spu_FwriteByIO), every voice given volume 0, pitch 0x3FFF, start address
   0x200 and ADSR 0, then keyed on and, four _spu_Fw1ts later, off, with four
   more to finish.  Last _spu_inTransfer = 1 (idle), SPUCNT = 0xC000 (enable,
   unmute) and both callbacks cleared.  Returns 0.  Caller: _SpuInit
   . */
/* func_8009DE78 */
s32 _spu_init(s32 bHot)
{
    s32 i;
    u32 n;

    *D_800B7BC0 |= 0xB0000;
    D_800B7BB0->rxx.main_vol.left = 0;
    D_800B7BB0->rxx.main_vol.right = 0;
    D_800B7BB0->rxx.spucnt = 0;
    D_800B7BCC = 0;
    D_800B7BD0 = 0;
    D_800B7BC8 = 0;
    _spu_Fw1ts();
    D_800B7BB0->rxx.main_vol.left = 0;
    D_800B7BB0->rxx.main_vol.right = 0;
    n = 0;
    while (D_800B7BB0->rxx.spustat & 0x7FF) {
        if (++n > 0xF00) {
            printf(D_800138D4, D_800138E4);
            break;
        }
    }
    D_800B7BD4 = 2;
    D_800B7BD8 = 3;
    D_800B7BDC = 8;
    D_800B7BE0 = 7;
    D_800B7BB0->rxx.data_trans = 4;
    D_800B7BB0->rxx.rev_vol.left = 0;
    D_800B7BB0->rxx.rev_vol.right = 0;
    D_800B7BB0->rxx.key_off[0] = 0xFFFF;
    D_800B7BB0->rxx.key_off[1] = 0xFFFF;
    D_800B7BB0->rxx.rev_mode[0] = 0;
    D_800B7BB0->rxx.rev_mode[1] = 0;
    for (i = 0; i < 10; i++) {
        D_800EA8C0[i] = 0;
    }
    if (bHot == 0) {
        D_800B7BB0->rxx.chan_fm[0] = 0;
        D_800B7BB0->rxx.chan_fm[1] = 0;
        D_800B7BB0->rxx.noise_mode[0] = 0;
        D_800B7BB0->rxx.noise_mode[1] = 0;
        D_800B7BB0->rxx.cd_vol.left = 0;
        D_800B7BB0->rxx.cd_vol.right = 0;
        D_800B7BB0->rxx.ex_vol.left = 0;
        D_800B7BB0->rxx.ex_vol.right = 0;
        D_800B7BC8 = 0x200;
        func_8009E0F8(D_800B7BF0, 0x10);
        for (i = 0; i < 24; i++) {
            D_800B7BB0->rxx.voice[i].volL = 0;
            D_800B7BB0->rxx.voice[i].volR = 0;
            D_800B7BB0->rxx.voice[i].pitch = 0x3FFF;
            D_800B7BB0->rxx.voice[i].addr = 0x200;
            D_800B7BB0->rxx.voice[i].adsr1 = 0;
            D_800B7BB0->rxx.voice[i].adsr2 = 0;
        }
        D_800B7BB0->rxx.key_on[0] = 0xFFFF;
        D_800B7BB0->rxx.key_on[1] = 0xFF;
        _spu_Fw1ts();
        _spu_Fw1ts();
        _spu_Fw1ts();
        _spu_Fw1ts();
        D_800B7BB0->rxx.key_off[0] = 0xFFFF;
        D_800B7BB0->rxx.key_off[1] = 0xFF;
        _spu_Fw1ts();
        _spu_Fw1ts();
        _spu_Fw1ts();
        _spu_Fw1ts();
    }
    D_800B7BE4 = 1;
    D_800B7BB0->rxx.spucnt = 0xC000;
    D_800B7BE8 = 0;
    D_800B7BEC = 0;
    return 0;
}
/* cc97_lib: func_8009E0F8 */
/* return_slot_fill: func_8009E0F8 */
/* reorder_fill: func_8009E0F8 */
/* far_syms: D_800B7BB0 D_800B7BC8 */
extern char D_800138F4[];                   /* "wait (wrdy H -> L)" */
extern char D_80013908[];                   /* "wait (dmaf clear/W)" */

/* libspu _spu_FwriteByIO (SPU.OBJ, static): write size bytes at addr to SPU
   RAM through the data port, without DMA.  Keeps SPUSTAT's low eleven bits,
   sets the transfer address register to _spu_tsa (D_800B7BC8) and waits one
   _spu_Fw1ts.  Then per block of at most 0x40 bytes: the words go into the
   transfer FIFO, SPUCNT's transfer mode (bits 4-5) is set to 1, manual
   write, and after one _spu_Fw1ts SPUSTAT bit 10 (busy) is polled until it
   clears (0xF01 polls, then the "wait (wrdy H -> L)" time-out print); two
   more _spu_Fw1ts end the block.  Afterwards the transfer mode goes back to
   0 and SPUSTAT is polled until its low eleven bits equal the value kept at
   entry ("wait (dmaf clear/W)" on time-out).  Callers: _spu_init (the dummy
   block) and _spu_Fw when _spu_transMode is not 0. */
void func_8009E0F8(u8 *addr, u32 size)
{
    u16 spustat;
    s32 num_to_trans;
    u16 *cur_pos;
    s32 i;
    u32 n;
    u16 cnt;

    cur_pos = (u16 *)addr;
    spustat = D_800B7BB0->rxx.spustat & 0x7FF;
    D_800B7BB0->rxx.trans_addr = D_800B7BC8;
    _spu_Fw1ts();
    while (size != 0) {
        num_to_trans = (size > 0x40) ? 0x40 : size;
        for (i = 0; i < num_to_trans; i += 2) {
            D_800B7BB0->rxx.trans_fifo = *cur_pos++;
        }
        cnt = D_800B7BB0->rxx.spucnt;
        cnt &= ~0x30;
        cnt |= 0x10;
        D_800B7BB0->rxx.spucnt = cnt;
        _spu_Fw1ts();
        n = 0;
        while (D_800B7BB0->rxx.spustat & 0x400) {
            if (++n > 0xF00) {
                printf(D_800138D4, D_800138F4);
                break;
            }
        }
        _spu_Fw1ts();
        _spu_Fw1ts();
        size -= num_to_trans;
    }
    cnt = D_800B7BB0->rxx.spucnt;
    cnt &= ~0x30;
    D_800B7BB0->rxx.spucnt = cnt;
    n = 0;
    while ((D_800B7BB0->rxx.spustat & 0x7FF) != spustat) {
        if (++n > 0xF00) {
            printf(D_800138D4, D_80013908);
            break;
        }
    }
}
