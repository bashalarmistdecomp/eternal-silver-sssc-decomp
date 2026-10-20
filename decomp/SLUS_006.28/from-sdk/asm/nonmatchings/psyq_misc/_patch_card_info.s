.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* Handwritten function */
/* libcard _patch_card_info: PSY-Q 4.5, lib/libcard.lib, member PATCH, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md).  A `.byte 0` run stands where the linker
   fills a field; the .LIB file holds a placeholder there, which is not read. */
nonmatching _patch_card_info, 0x44

glabel _patch_card_info
    .reloc _patch_card_info + 0x0, R_MIPS_HI16, D_800EA910
    .reloc _patch_card_info + 0x4, R_MIPS_LO16, D_800EA910
    .reloc _patch_card_info + 0x28, R_MIPS_26, FlushCache
    .reloc _patch_card_info + 0x30, R_MIPS_HI16, D_800EA910
    .reloc _patch_card_info + 0x34, R_MIPS_LO16, D_800EA910
    .byte 0, 0
    .incbin "lib/libcard.lib", 0xA24, 0x2
    .byte 0, 0
    .incbin "lib/libcard.lib", 0xA28, 0x22
    .byte 0, 0, 0
    .incbin "lib/libcard.lib", 0xA4D, 0x5
    .byte 0, 0
    .incbin "lib/libcard.lib", 0xA54, 0x2
    .byte 0, 0
    .incbin "lib/libcard.lib", 0xA58, 0xE
endlabel _patch_card_info
