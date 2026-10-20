.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libapi write: PSY-Q 4.5, lib/libapi.lib, member A53, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching write, 0xC

glabel write
    .incbin "lib/libapi.lib", 0x1E86, 0xC
endlabel write
    .space 0x4
