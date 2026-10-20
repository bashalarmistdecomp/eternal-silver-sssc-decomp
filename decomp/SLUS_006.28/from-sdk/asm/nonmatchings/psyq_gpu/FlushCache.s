.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libapi FlushCache: PSY-Q 4.5, lib/libapi.lib, member C68, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching FlushCache, 0xC

glabel FlushCache
    .incbin "lib/libapi.lib", 0x374, 0xC
endlabel FlushCache
    .space 0x4
