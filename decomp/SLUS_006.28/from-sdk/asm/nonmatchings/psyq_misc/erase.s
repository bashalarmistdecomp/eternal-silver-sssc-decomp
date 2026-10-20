.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libapi erase: PSY-Q 4.5, lib/libapi.lib, member A69, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching erase, 0xC

glabel erase
    .incbin "lib/libapi.lib", 0x232C, 0xC
endlabel erase
    .space 0x4
