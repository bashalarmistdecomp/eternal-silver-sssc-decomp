.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libapi open: PSY-Q 4.5, lib/libapi.lib, member A50, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching open, 0xC

glabel open
    .incbin "lib/libapi.lib", 0x1CCF, 0xC
endlabel open
    .space 0x4
