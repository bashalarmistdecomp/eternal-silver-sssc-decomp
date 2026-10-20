.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libapi _bu_init: PSY-Q 4.5, lib/libapi.lib, member C112, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching _bu_init, 0xC

glabel _bu_init
    .incbin "lib/libapi.lib", 0x5D4, 0xC
endlabel _bu_init
    .space 0x4
