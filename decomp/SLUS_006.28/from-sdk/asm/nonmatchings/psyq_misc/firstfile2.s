.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libapi firstfile2: PSY-Q 4.5, lib/libapi.lib, member A66, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching firstfile2, 0xC

glabel firstfile2
    .incbin "lib/libapi.lib", 0x216B, 0xC
endlabel firstfile2
    .space 0x4
