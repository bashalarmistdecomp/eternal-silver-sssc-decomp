.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* Handwritten function */
/* libapi _remove_ChgclrPAD: PSY-Q 4.5, lib/libapi.lib, member CHCLRPAD, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md).  A `.byte 0` run stands where the linker
   fills a field; the .LIB file holds a placeholder there, which is not read. */
nonmatching _remove_ChgclrPAD, 0x68

glabel _remove_ChgclrPAD
    .reloc _remove_ChgclrPAD + 0x0, R_MIPS_HI16, D_800EA900
    .reloc _remove_ChgclrPAD + 0x4, R_MIPS_LO16, D_800EA900
    .reloc _remove_ChgclrPAD + 0x8, R_MIPS_26, EnterCriticalSection
    .reloc _remove_ChgclrPAD + 0x44, R_MIPS_26, FlushCache
    .reloc _remove_ChgclrPAD + 0x4C, R_MIPS_26, ExitCriticalSection
    .reloc _remove_ChgclrPAD + 0x54, R_MIPS_HI16, D_800EA900
    .reloc _remove_ChgclrPAD + 0x58, R_MIPS_LO16, D_800EA900
    .byte 0, 0
    .incbin "lib/libapi.lib", 0x4DE9, 0x2
    .byte 0, 0
    .incbin "lib/libapi.lib", 0x4DED, 0x2
    .byte 0, 0, 0
    .incbin "lib/libapi.lib", 0x4DF2, 0x39
    .byte 0, 0, 0
    .incbin "lib/libapi.lib", 0x4E2E, 0x5
    .byte 0, 0, 0
    .incbin "lib/libapi.lib", 0x4E36, 0x5
    .byte 0, 0
    .incbin "lib/libapi.lib", 0x4E3D, 0x2
    .byte 0, 0
    .incbin "lib/libapi.lib", 0x4E41, 0xE
endlabel _remove_ChgclrPAD
    .space 0x8
