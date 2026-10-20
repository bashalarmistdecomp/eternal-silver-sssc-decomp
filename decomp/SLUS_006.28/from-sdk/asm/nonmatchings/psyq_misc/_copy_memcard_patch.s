.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libcard _copy_memcard_patch: PSY-Q 4.5, lib/libcard.lib, member PATCH, .text + 0x1E0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md).  A `.byte 0` run stands where the linker
   fills a field; the .LIB file holds a placeholder there, which is not read. */
nonmatching _copy_memcard_patch, 0x34

glabel _copy_memcard_patch
    .reloc _copy_memcard_patch + 0x4, R_MIPS_HI16, func_800A169C
    .reloc _copy_memcard_patch + 0x8, R_MIPS_LO16, func_800A169C
    .reloc _copy_memcard_patch + 0xC, R_MIPS_HI16, func_800A170C
    .reloc _copy_memcard_patch + 0x10, R_MIPS_LO16, func_800A170C
    .incbin "lib/libcard.lib", 0xC02, 0x4
    .byte 0, 0
    .incbin "lib/libcard.lib", 0xC08, 0x2
    .byte 0, 0
    .incbin "lib/libcard.lib", 0xC0C, 0x2
    .byte 0, 0
    .incbin "lib/libcard.lib", 0xC10, 0x2
    .byte 0, 0
    .incbin "lib/libcard.lib", 0xC14, 0x22
endlabel _copy_memcard_patch
    .space 0xC
