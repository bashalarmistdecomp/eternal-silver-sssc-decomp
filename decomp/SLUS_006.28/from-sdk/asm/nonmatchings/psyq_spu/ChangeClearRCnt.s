.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libapi ChangeClearRCnt: PSY-Q 4.5, lib/libapi.lib, member L10, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching ChangeClearRCnt, 0xC

glabel ChangeClearRCnt
    .incbin "lib/libapi.lib", 0x2B9E, 0xC
endlabel ChangeClearRCnt
    .space 0x4
