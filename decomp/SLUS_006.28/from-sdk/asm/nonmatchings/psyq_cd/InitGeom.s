.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* Handwritten function */
/* libgte InitGeom: PSY-Q 4.5, lib/libgte.lib, member MSC00, .text + 0x8.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md).  A `.byte 0` run stands where the linker
   fills a field; the .LIB file holds a placeholder there, which is not read. */
nonmatching InitGeom, 0x80

glabel InitGeom
    .reloc InitGeom + 0x0, R_MIPS_HI16, D_800B6120
    .reloc InitGeom + 0x4, R_MIPS_LO16, D_800B6120
    .reloc InitGeom + 0x8, R_MIPS_26, _patch_gte
    .reloc InitGeom + 0x10, R_MIPS_HI16, D_800B6120
    .reloc InitGeom + 0x14, R_MIPS_LO16, D_800B6120
    .byte 0, 0
    .incbin "lib/libgte.lib", 0x4C56, 0x2
    .byte 0, 0
    .incbin "lib/libgte.lib", 0x4C5A, 0x2
    .byte 0, 0, 0
    .incbin "lib/libgte.lib", 0x4C5F, 0x5
    .byte 0, 0
    .incbin "lib/libgte.lib", 0x4C66, 0x2
    .byte 0, 0
    .incbin "lib/libgte.lib", 0x4C6A, 0x6A
endlabel InitGeom
    .space 0x8
