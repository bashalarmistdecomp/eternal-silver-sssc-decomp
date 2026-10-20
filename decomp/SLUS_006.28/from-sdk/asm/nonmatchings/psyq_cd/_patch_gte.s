.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libgte _patch_gte: PSY-Q 4.5, lib/libgte.lib, member PATCHGTE, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md).  A `.byte 0` run stands where the linker
   fills a field; the .LIB file holds a placeholder there, which is not read. */
nonmatching _patch_gte, 0xAC

glabel _patch_gte
    .reloc _patch_gte + 0x0, R_MIPS_HI16, D_800E70B0
    .reloc _patch_gte + 0x4, R_MIPS_LO16, D_800E70B0
    .reloc _patch_gte + 0x8, R_MIPS_26, EnterCriticalSection
    .reloc _patch_gte + 0x30, R_MIPS_HI16, func_80090044
    .reloc _patch_gte + 0x34, R_MIPS_LO16, func_80090044
    .reloc _patch_gte + 0x38, R_MIPS_HI16, D_8009005C
    .reloc _patch_gte + 0x3C, R_MIPS_LO16, D_8009005C
    .reloc _patch_gte + 0x60, R_MIPS_HI16, D_8009005C
    .reloc _patch_gte + 0x64, R_MIPS_LO16, D_8009005C
    .reloc _patch_gte + 0x68, R_MIPS_HI16, D_80090074
    .reloc _patch_gte + 0x6C, R_MIPS_LO16, D_80090074
    .reloc _patch_gte + 0x88, R_MIPS_26, FlushCache
    .reloc _patch_gte + 0x90, R_MIPS_26, ExitCriticalSection
    .reloc _patch_gte + 0x98, R_MIPS_HI16, D_800E70B0
    .reloc _patch_gte + 0x9C, R_MIPS_LO16, D_800E70B0
    .byte 0, 0
    .incbin "lib/libgte.lib", 0x46870, 0x2
    .byte 0, 0
    .incbin "lib/libgte.lib", 0x46874, 0x2
    .byte 0, 0, 0
    .incbin "lib/libgte.lib", 0x46879, 0x25
    .byte 0, 0
    .incbin "lib/libgte.lib", 0x468A0, 0x2
    .byte 0, 0
    .incbin "lib/libgte.lib", 0x468A4, 0x2
    .byte 0, 0
    .incbin "lib/libgte.lib", 0x468A8, 0x2
    .byte 0, 0
    .incbin "lib/libgte.lib", 0x468AC, 0x22
    .byte 0, 0
    .incbin "lib/libgte.lib", 0x468D0, 0x2
    .byte 0, 0
    .incbin "lib/libgte.lib", 0x468D4, 0x2
    .byte 0, 0
    .incbin "lib/libgte.lib", 0x468D8, 0x2
    .byte 0, 0
    .incbin "lib/libgte.lib", 0x468DC, 0x1A
    .byte 0, 0, 0
    .incbin "lib/libgte.lib", 0x468F9, 0x5
    .byte 0, 0, 0
    .incbin "lib/libgte.lib", 0x46901, 0x5
    .byte 0, 0
    .incbin "lib/libgte.lib", 0x46908, 0x2
    .byte 0, 0
    .incbin "lib/libgte.lib", 0x4690C, 0xE
endlabel _patch_gte
