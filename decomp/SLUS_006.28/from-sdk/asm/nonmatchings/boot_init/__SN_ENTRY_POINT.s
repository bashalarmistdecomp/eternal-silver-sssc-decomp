.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* 2mbyte.obj __SN_ENTRY_POINT: PSY-Q 4.5, lib/2mbyte.obj, .text + 0x8.
   These bytes are read from Sony's own file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md).  A `.byte 0` run stands where the linker
   fills a field; Sony's file holds a placeholder there, which is not read. */
nonmatching __SN_ENTRY_POINT

glabel __SN_ENTRY_POINT
    .reloc __SN_ENTRY_POINT + 0x0, R_MIPS_HI16, D_800B8820
    .reloc __SN_ENTRY_POINT + 0x4, R_MIPS_LO16, D_800B8820
    .reloc __SN_ENTRY_POINT + 0x8, R_MIPS_HI16, D_800EAFE8
    .reloc __SN_ENTRY_POINT + 0xC, R_MIPS_LO16, D_800EAFE8
    .reloc __SN_ENTRY_POINT + 0x38, R_MIPS_HI16, D_80013CF8
    .reloc __SN_ENTRY_POINT + 0x3C, R_MIPS_LO16, D_80013CF8
    .reloc __SN_ENTRY_POINT + 0x50, R_MIPS_HI16, D_800EAFE8
    .reloc __SN_ENTRY_POINT + 0x54, R_MIPS_LO16, D_800EAFE8
    .reloc __SN_ENTRY_POINT + 0x60, R_MIPS_HI16, D_800B8714
    .reloc __SN_ENTRY_POINT + 0x64, R_MIPS_LO16, D_800B8714
    .reloc __SN_ENTRY_POINT + 0x78, R_MIPS_HI16, D_800B8820
    .reloc __SN_ENTRY_POINT + 0x7C, R_MIPS_LO16, D_800B8820
    .reloc __SN_ENTRY_POINT + 0x80, R_MIPS_HI16, _gp
    .reloc __SN_ENTRY_POINT + 0x84, R_MIPS_LO16, _gp
    .reloc __SN_ENTRY_POINT + 0x8C, R_MIPS_26, InitHeap
    .reloc __SN_ENTRY_POINT + 0x94, R_MIPS_HI16, D_800B8820
    .reloc __SN_ENTRY_POINT + 0x98, R_MIPS_LO16, D_800B8820
    .reloc __SN_ENTRY_POINT + 0xA0, R_MIPS_26, main
    .byte 0, 0
    .incbin "lib/2mbyte.obj", 0x71, 0x2
    .byte 0, 0
    .incbin "lib/2mbyte.obj", 0x75, 0x2
    .byte 0, 0
    .incbin "lib/2mbyte.obj", 0x79, 0x2
    .byte 0, 0
    .incbin "lib/2mbyte.obj", 0x7D, 0x2A
    .byte 0, 0
    .incbin "lib/2mbyte.obj", 0xA9, 0x2
    .byte 0, 0
    .incbin "lib/2mbyte.obj", 0xAD, 0x12
    .byte 0, 0
    .incbin "lib/2mbyte.obj", 0xC1, 0x2
    .byte 0, 0
    .incbin "lib/2mbyte.obj", 0xC5, 0xA
    .byte 0, 0
    .incbin "lib/2mbyte.obj", 0xD1, 0x2
    .byte 0, 0
    .incbin "lib/2mbyte.obj", 0xD5, 0x12
    .byte 0, 0
    .incbin "lib/2mbyte.obj", 0xE9, 0x2
    .byte 0, 0
    .incbin "lib/2mbyte.obj", 0xED, 0x2
    .byte 0, 0
    .incbin "lib/2mbyte.obj", 0xF1, 0x2
    .byte 0, 0
    .incbin "lib/2mbyte.obj", 0xF5, 0x6
    .byte 0, 0, 0
    .incbin "lib/2mbyte.obj", 0xFE, 0x5
    .byte 0, 0
    .incbin "lib/2mbyte.obj", 0x105, 0x2
    .byte 0, 0
    .incbin "lib/2mbyte.obj", 0x109, 0x6
    .byte 0, 0, 0
    .incbin "lib/2mbyte.obj", 0x112, 0x9
  alabel D_80013CF8
    .incbin "lib/2mbyte.obj", 0x11B, 0x10
enddlabel __SN_ENTRY_POINT
