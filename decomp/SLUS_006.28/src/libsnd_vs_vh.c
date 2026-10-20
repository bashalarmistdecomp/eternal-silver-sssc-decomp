/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/vs_vh.c) and psyz (decomp/src/libsnd/vs_vh.c), all
   MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member vs_vh.o: SsVabOpenHeadSticky, SsVabFakeHead,
   func_8009D7F0, _SsVabOpenHeadWithMode. The first split had it in
   psyq_misc_s.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* libsnd's per-voice state (_svm_voice[24], 0x38 each). */
/* libsnd's current-note parameters (_svm_cur). */
/* libsnd's per-score sequence state (_ss_score[seq][sep], 0xB0 each). */
extern u8 D_800EA710[];   /* _svm_vab_used[16] */
extern s32 D_800EA728[];
extern ProgAtr *D_800EA608[];
extern VabHdr *D_800EA648[];
extern VagAtr *D_800EA690[];
extern s16 D_800EA6D2;
extern s32 D_800EA770[];
extern u16 D_800EA768;
void _spu_setInTransfer(s32);
s32 _spu_getInTransfer(void);
s16 _SsVabOpenHeadWithMode(u8 *addr, s16 vabid, s32 (*alloc)(s32, s32, s16), s32 sbaddr);

/* cc97_lib: SsVabOpenHeadSticky */
/* gnu_as_loads: SsVabOpenHeadSticky */
s32 func_8009D7F0(s32 size, s32 sbaddr, s16 vabId);  /* TU defines this below */

/* libsnd SsVabOpenHeadSticky (VS_VH.OBJ): register the VAB header at addr in
   slot vabid (-1 = the first free one) with its samples at the caller's SPU
   address sbaddr: _SsVabOpenHeadWithMode with the object's
   pass-through allocator (func_8009D7F0, which returns sbaddr).  Returns
   the slot, or -1. */
/* func_8009D788 */
short SsVabOpenHeadSticky(unsigned char *addr, short vabid, unsigned long sbaddr)
{
    return _SsVabOpenHeadWithMode(addr, vabid, func_8009D7F0, sbaddr);
}
/* cc97_lib: SsVabFakeHead */
/* gnu_as_loads: SsVabFakeHead */

/* libsnd SsVabFakeHead (VS_VH.OBJ): open the VAB header at addr in slot vabid
   (-1 = the first free of 16) for a sample body that is already in SPU RAM at
   sbaddr.  It is _SsVabOpenHeadWithMode with func_8009D7F0 as
   the allocator, which reserves nothing and hands sbaddr back as the block's
   address.  Returns the slot, or -1.  SsVabOpenHeadSticky (the function above) is the same thirteen words; the two are told apart by
   their place in the object. */
/* func_8009D7BC */
short SsVabFakeHead(unsigned char *addr, short vabid, unsigned long sbaddr)
{
    return _SsVabOpenHeadWithMode(addr, vabid, func_8009D7F0, sbaddr);
}
/* cc97_lib: func_8009D7F0 */
/* gnu_as_loads: func_8009D7F0 */

/* libsnd VS_VH.OBJ's static allocator, the one SsVabOpenHeadSticky and
   SsVabFakeHead hand to _SsVabOpenHeadWithMode: it reserves nothing and
   returns the caller's SPU address as the block.  Its parameter list is
   the allocator pointer's (size, sbaddr, slot), like func_8009D71C's; the
   two-instruction body reads only sbaddr. */
s32 func_8009D7F0(s32 size, s32 sbaddr, s16 vabId)
{
    return sbaddr;
}
/* cc97_lib: _SsVabOpenHeadWithMode */
/* gnu_as_loads: _SsVabOpenHeadWithMode */
/* return_slot_fill: _SsVabOpenHeadWithMode */
extern s32 D_800EA6E8;    /* _svm_vab_not_send_size */

/* libsnd _SsVabOpenHeadWithMode, the body of SsVabOpenHead /
   SsVabOpenHeadSticky / SsVabFakeHead: register the VAB header at addr in
   slot vabid (-1 = the first free of 16) and reserve SPU RAM for its
   samples.  Refuses (-1) while a transfer is pending (_spu_getInTransfer()
   == 1); otherwise sets the transfer flag, takes the slot (_svm_vab_used =
   1, _svm_vab_count++) and stores the header pointer (_svm_vab_vh).  The
   header must be 'VABp' (form >> 8 == "VAB"); a 'p' form of version 5 or
   later has 128 program slots (kMaxPrograms), anything else 64, and the
   header's ps may not exceed that.  The program table follows the 0x20-byte
   header (_svm_vab_pg); each ProgAtr.reserved1 becomes the program's index
   among the programs that have tones.  The tone table follows the program
   table (_svm_vab_tn), 16 VagAtr (0x200 bytes) per used program, then the
   256-entry u16 VAG size table: sizes are in units of 8 bytes from version
   5, 4 before.  The sum of entries 0..vs, rounded up to 64, goes to alloc
   (size, sbaddr, slot), which returns the SPU address of the block, or -1
   (returned as is).  A block ending past 0x80000 fails.  Then
   _svm_vab_start is the block, each VAG's end address >> 3 is written into
   ProgAtr.reserved2 of record n / 2 (low half for even n, high half for
   odd), _svm_vab_total is the unrounded sum and the slot's state becomes 2
   (header opened, body not yet sent).  Returns the slot.  The three
   failures after the slot is taken free it, clear the transfer flag and
   undo the count. */
/* func_8009D7F8 */
s16 _SsVabOpenHeadWithMode(u8 *addr, s16 vabid, s32 (*alloc)(s32, s32, s16), s32 sbaddr)
{
    int vagLens[256];
    s32 i;
    s32 total;
    s16 vabId_2;
    u16 len;
    u16 *vagOff;
    u32 magic;
    u32 spuAddr;
    u8 numVags;
    ProgAtr *pg;
    u8 *p;
    VabHdr *vh;

    vabId_2 = 16;
    if (_spu_getInTransfer() == 1) {
        return -1;
    }
    _spu_setInTransfer(1);
    if (vabid >= 16) {
        _spu_setInTransfer(0);
        return -1;
    }
    if (vabid == -1) {
        for (i = 0; i < 16; i++) {
            if (D_800EA710[i] == 0) {
                D_800EA710[i] = 1;
                vabId_2 = i;
                D_800EA768++;
                break;
            }
        }
    } else {
        if (D_800EA710[vabid] == 0) {
            D_800EA710[vabid] = 1;
            vabId_2 = vabid;
            D_800EA768++;
        }
    }
    if (vabId_2 >= 16) {
        _spu_setInTransfer(0);
        return -1;
    }
    p = addr;
    D_800EA648[vabId_2] = (VabHdr *)p;
    D_800EA6E8 = 0;
    p += 0x20;
    vh = (VabHdr *)addr;
    magic = vh->form;
    if ((magic >> 8) != 0x564142) {
        D_800EA710[vabId_2] = 0;
        _spu_setInTransfer(0);
        D_800EA768--;
        return -1;
    }
    if ((magic & 0xFF) == 'p') {
        if (vh->ver >= 5) {
            D_800EA6D2 = 0x80;
        } else {
            D_800EA6D2 = 0x40;
        }
    } else {
        D_800EA6D2 = 0x40;
    }
    if (vh->ps > D_800EA6D2) {
        D_800EA710[vabId_2] = 0;
        _spu_setInTransfer(0);
        D_800EA768--;
        return -1;
    }
    D_800EA608[vabId_2] = (ProgAtr *)p;
    pg = (ProgAtr *)p;
    p += D_800EA6D2 * 0x10;
    total = 0;
    for (i = 0; i < D_800EA6D2; i++) {
        pg[i].reserved1 = total;
        if (pg[i].tones != 0) {
            total++;
        }
    }
    total = 0;
    D_800EA690[vabId_2] = (VagAtr *)p;
    vagOff = (u16 *)(p + (vh->ps << 9));
    numVags = vh->vs;
    for (i = 0; i < 256; i++) {
        if (numVags >= i) {
            len = *vagOff;
            if (vh->ver >= 5) {
                vagLens[i] = len * 8;
            } else {
                vagLens[i] = len * 4;
            }
            total += vagLens[i];
        }
        vagOff++;
    }
    total = (total + 0x3F) & ~0x3F;
    spuAddr = alloc(total, sbaddr, vabId_2);
    if (spuAddr == -1) {
        return -1;
    }
    if ((spuAddr + total) > 0x80000U) {
        D_800EA710[vabId_2] = 0;
        _spu_setInTransfer(0);
        D_800EA768--;
        return -1;
    }
    D_800EA770[vabId_2] = spuAddr;
    total = 0;
    for (i = 0; i <= numVags; i++) {
        total += vagLens[i];
        if (!(i & 1)) {
            AS(SvmVagAddrs, pg[i / 2].reserved2).odd = (spuAddr + total) >> 3;
        } else {
            AS(SvmVagAddrs, pg[i / 2].reserved2).even = (spuAddr + total) >> 3;
        }
    }
    D_800EA728[vabId_2] = total;
    D_800EA710[vabId_2] = 2;
    return vabId_2;
}

INCLUDE_PAD(4);
