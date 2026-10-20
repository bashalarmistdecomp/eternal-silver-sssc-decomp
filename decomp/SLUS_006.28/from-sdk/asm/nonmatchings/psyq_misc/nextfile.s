.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libapi nextfile: PSY-Q 4.5, lib/libapi.lib, member A67, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching nextfile, 0xC

glabel nextfile
    .incbin "lib/libapi.lib", 0x2204, 0xC
endlabel nextfile
    .space 0x4
