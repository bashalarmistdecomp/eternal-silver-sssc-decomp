.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libcard _ExitCard: PSY-Q 4.5, lib/libcard.lib, member END, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md).  A `.byte 0` run stands where the linker
   fills a field; the .LIB file holds a placeholder there, which is not read. */
nonmatching _ExitCard, 0x70

glabel _ExitCard
    .reloc _ExitCard + 0x0, R_MIPS_HI16, D_800EA920
    .reloc _ExitCard + 0x4, R_MIPS_LO16, D_800EA920
    .reloc _ExitCard + 0x8, R_MIPS_26, EnterCriticalSection
    .reloc _ExitCard + 0x24, R_MIPS_HI16, D_800A18E8
    .reloc _ExitCard + 0x28, R_MIPS_LO16, D_800A18E8
    .reloc _ExitCard + 0x2C, R_MIPS_HI16, D_800A18F4
    .reloc _ExitCard + 0x30, R_MIPS_LO16, D_800A18F4
    .reloc _ExitCard + 0x4C, R_MIPS_26, FlushCache
    .reloc _ExitCard + 0x54, R_MIPS_26, ExitCriticalSection
    .reloc _ExitCard + 0x5C, R_MIPS_HI16, D_800EA920
    .reloc _ExitCard + 0x60, R_MIPS_LO16, D_800EA920
    .byte 0, 0
    .incbin "lib/libcard.lib", 0x1147, 0x2
    .byte 0, 0
    .incbin "lib/libcard.lib", 0x114B, 0x2
    .byte 0, 0, 0
    .incbin "lib/libcard.lib", 0x1150, 0x19
    .byte 0, 0
    .incbin "lib/libcard.lib", 0x116B, 0x2
    .byte 0, 0
    .incbin "lib/libcard.lib", 0x116F, 0x2
    .byte 0, 0
    .incbin "lib/libcard.lib", 0x1173, 0x2
    .byte 0, 0
    .incbin "lib/libcard.lib", 0x1177, 0x1A
    .byte 0, 0, 0
    .incbin "lib/libcard.lib", 0x1194, 0x5
    .byte 0, 0, 0
    .incbin "lib/libcard.lib", 0x119C, 0x5
    .byte 0, 0
    .incbin "lib/libcard.lib", 0x11A3, 0x2
    .byte 0, 0
    .incbin "lib/libcard.lib", 0x11A7, 0xE
endlabel _ExitCard
  alabel D_800A18E8
    .space 0xC
  alabel D_800A18F4
    .space 0x4
