.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libapi close: PSY-Q 4.5, lib/libapi.lib, member A54, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching close, 0xC

glabel close
    .incbin "lib/libapi.lib", 0x1F19, 0xC
endlabel close
    .space 0x4
