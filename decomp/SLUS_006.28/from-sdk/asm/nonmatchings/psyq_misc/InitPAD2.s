.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libapi InitPAD2: PSY-Q 4.5, lib/libapi.lib, member A18, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching InitPAD2, 0xC

glabel InitPAD2
    .incbin "lib/libapi.lib", 0x132C, 0xC
endlabel InitPAD2
    .space 0x4
