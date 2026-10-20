.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libcard _patch_card: PSY-Q 4.5, lib/libcard.lib, member PATCH, .text + 0xDC.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md).  A `.byte 0` run stands where the linker
   fills a field; the .LIB file holds a placeholder there, which is not read. */
nonmatching _patch_card, 0x94

glabel _patch_card
    .reloc _patch_card + 0x0, R_MIPS_HI16, D_800EA910
    .reloc _patch_card + 0x4, R_MIPS_LO16, D_800EA910
    .reloc _patch_card + 0x8, R_MIPS_26, EnterCriticalSection
    .reloc _patch_card + 0x4C, R_MIPS_HI16, func_800A170C
    .reloc _patch_card + 0x50, R_MIPS_LO16, func_800A170C
    .reloc _patch_card + 0x54, R_MIPS_HI16, func_800A170C + 0x14
    .reloc _patch_card + 0x58, R_MIPS_LO16, func_800A170C + 0x14
    .reloc _patch_card + 0x78, R_MIPS_26, FlushCache
    .reloc _patch_card + 0x80, R_MIPS_HI16, D_800EA910
    .reloc _patch_card + 0x84, R_MIPS_LO16, D_800EA910
    .byte 0, 0
    .incbin "lib/libcard.lib", 0xB00, 0x2
    .byte 0, 0
    .incbin "lib/libcard.lib", 0xB04, 0x2
    .byte 0, 0, 0
    .incbin "lib/libcard.lib", 0xB09, 0x41
    .byte 0, 0
    .incbin "lib/libcard.lib", 0xB4C, 0x2
    .byte 0, 0
    .incbin "lib/libcard.lib", 0xB50, 0x2
    .byte 0, 0
    .incbin "lib/libcard.lib", 0xB54, 0x2
    .byte 0, 0
    .incbin "lib/libcard.lib", 0xB58, 0x1E
    .byte 0, 0, 0
    .incbin "lib/libcard.lib", 0xB79, 0x5
    .byte 0, 0
    .incbin "lib/libcard.lib", 0xB80, 0x2
    .byte 0, 0
    .incbin "lib/libcard.lib", 0xB84, 0xE
endlabel _patch_card
