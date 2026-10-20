.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libapi ChangeClearPAD: PSY-Q 4.5, lib/libapi.lib, member A91, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching ChangeClearPAD, 0xC

glabel ChangeClearPAD
    .incbin "lib/libapi.lib", 0x276D, 0xC
endlabel ChangeClearPAD
    .space 0x4
