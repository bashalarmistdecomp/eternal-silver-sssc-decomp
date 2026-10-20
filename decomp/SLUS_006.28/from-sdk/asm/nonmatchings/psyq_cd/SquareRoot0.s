.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* Handwritten function */
/* libgte SquareRoot0: PSY-Q 4.5, lib/libgte.lib, member MSC01, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md).  A `.byte 0` run stands where the linker
   fills a field; the .LIB file holds a placeholder there, which is not read. */
nonmatching SquareRoot0, 0x84

glabel SquareRoot0
    .reloc SquareRoot0 + 0x5C, R_MIPS_HI16, D_800B6130
    .reloc SquareRoot0 + 0x64, R_MIPS_LO16, D_800B6130
    .incbin "lib/libgte.lib", 0x4DA9, 0x5C
    .byte 0, 0
    .incbin "lib/libgte.lib", 0x4E07, 0x6
    .byte 0, 0
    .incbin "lib/libgte.lib", 0x4E0F, 0x1E
endlabel SquareRoot0
    .space 0xC
