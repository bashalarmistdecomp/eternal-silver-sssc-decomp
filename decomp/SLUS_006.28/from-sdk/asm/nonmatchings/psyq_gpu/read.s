.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libapi read: PSY-Q 4.5, lib/libapi.lib, member A52, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching read, 0xC

glabel read
    .incbin "lib/libapi.lib", 0x1DF3, 0xC
endlabel read
    .space 0x4
