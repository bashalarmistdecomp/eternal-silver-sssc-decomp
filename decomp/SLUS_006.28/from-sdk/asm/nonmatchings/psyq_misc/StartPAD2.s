.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libapi StartPAD2: PSY-Q 4.5, lib/libapi.lib, member A19, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching StartPAD2, 0xC

glabel StartPAD2
    .incbin "lib/libapi.lib", 0x13C7, 0xC
endlabel StartPAD2
    .space 0x4
