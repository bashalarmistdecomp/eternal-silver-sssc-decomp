.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libcard _patch_card2: PSY-Q 4.5, lib/libcard.lib, member PATCH, .text + 0x170.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md).  A `.byte 0` run stands where the linker
   fills a field; the .LIB file holds a placeholder there, which is not read. */
nonmatching _patch_card2, 0x70

glabel _patch_card2
    .reloc _patch_card2 + 0x0, R_MIPS_HI16, D_800EA910
    .reloc _patch_card2 + 0x4, R_MIPS_LO16, D_800EA910
    .reloc _patch_card2 + 0x8, R_MIPS_26, EnterCriticalSection
    .reloc _patch_card2 + 0x2C, R_MIPS_HI16, func_800A170C + 0x14
    .reloc _patch_card2 + 0x30, R_MIPS_LO16, func_800A170C + 0x14
    .reloc _patch_card2 + 0x34, R_MIPS_HI16, _patch_card
    .reloc _patch_card2 + 0x38, R_MIPS_LO16, _patch_card
    .reloc _patch_card2 + 0x54, R_MIPS_26, FlushCache
    .reloc _patch_card2 + 0x5C, R_MIPS_HI16, D_800EA910
    .reloc _patch_card2 + 0x60, R_MIPS_LO16, D_800EA910
    .byte 0, 0
    .incbin "lib/libcard.lib", 0xB94, 0x2
    .byte 0, 0
    .incbin "lib/libcard.lib", 0xB98, 0x2
    .byte 0, 0, 0
    .incbin "lib/libcard.lib", 0xB9D, 0x21
    .byte 0, 0
    .incbin "lib/libcard.lib", 0xBC0, 0x2
    .byte 0, 0
    .incbin "lib/libcard.lib", 0xBC4, 0x2
    .byte 0, 0
    .incbin "lib/libcard.lib", 0xBC8, 0x2
    .byte 0, 0
    .incbin "lib/libcard.lib", 0xBCC, 0x1A
    .byte 0, 0, 0
    .incbin "lib/libcard.lib", 0xBE9, 0x5
    .byte 0, 0
    .incbin "lib/libcard.lib", 0xBF0, 0x2
    .byte 0, 0
    .incbin "lib/libcard.lib", 0xBF4, 0xE
endlabel _patch_card2
