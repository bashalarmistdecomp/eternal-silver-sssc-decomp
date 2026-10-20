.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libapi DisablePAD: PSY-Q 4.5, lib/libapi.lib, member PATCH, .text + 0x14.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md).  A `.byte 0` run stands where the linker
   fills a field; the .LIB file holds a placeholder there, which is not read. */
nonmatching DisablePAD, 0x14

glabel DisablePAD
    .reloc DisablePAD + 0x0, R_MIPS_HI16, jtbl_800EA8FC_main
    .reloc DisablePAD + 0x4, R_MIPS_LO16, jtbl_800EA8FC_main
    .byte 0, 0
    .incbin "lib/libapi.lib", 0x34DF, 0x2
    .byte 0, 0
    .incbin "lib/libapi.lib", 0x34E3, 0xE
endlabel DisablePAD
