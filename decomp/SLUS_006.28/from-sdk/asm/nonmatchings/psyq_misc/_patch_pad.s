.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* Handwritten function */
/* libapi _patch_pad: PSY-Q 4.5, lib/libapi.lib, member PATCH, .text + 0x28.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md).  A `.byte 0` run stands where the linker
   fills a field; the .LIB file holds a placeholder there, which is not read. */
nonmatching _patch_pad, 0x70

glabel _patch_pad
    .reloc _patch_pad + 0x0, R_MIPS_HI16, D_800EA8F0
    .reloc _patch_pad + 0x4, R_MIPS_LO16, D_800EA8F0
    .reloc _patch_pad + 0x8, R_MIPS_26, EnterCriticalSection
    .reloc _patch_pad + 0x2C, R_MIPS_HI16, jtbl_800EA8F8_main
    .reloc _patch_pad + 0x30, R_MIPS_LO16, jtbl_800EA8F8_main
    .reloc _patch_pad + 0x38, R_MIPS_HI16, jtbl_800EA8FC_main
    .reloc _patch_pad + 0x3C, R_MIPS_LO16, jtbl_800EA8FC_main
    .reloc _patch_pad + 0x54, R_MIPS_26, FlushCache
    .reloc _patch_pad + 0x5C, R_MIPS_HI16, D_800EA8F0
    .reloc _patch_pad + 0x60, R_MIPS_LO16, D_800EA8F0
    .byte 0, 0
    .incbin "lib/libapi.lib", 0x34F3, 0x2
    .byte 0, 0
    .incbin "lib/libapi.lib", 0x34F7, 0x2
    .byte 0, 0, 0
    .incbin "lib/libapi.lib", 0x34FC, 0x21
    .byte 0, 0
    .incbin "lib/libapi.lib", 0x351F, 0x2
    .byte 0, 0
    .incbin "lib/libapi.lib", 0x3523, 0x6
    .byte 0, 0
    .incbin "lib/libapi.lib", 0x352B, 0x2
    .byte 0, 0
    .incbin "lib/libapi.lib", 0x352F, 0x16
    .byte 0, 0, 0
    .incbin "lib/libapi.lib", 0x3548, 0x5
    .byte 0, 0
    .incbin "lib/libapi.lib", 0x354F, 0x2
    .byte 0, 0
    .incbin "lib/libapi.lib", 0x3553, 0xE
endlabel _patch_pad
    .space 0x8
