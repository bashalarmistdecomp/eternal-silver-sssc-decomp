.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libapi StopPAD2: PSY-Q 4.5, lib/libapi.lib, member A20, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching StopPAD2, 0xC

glabel StopPAD2
    .incbin "lib/libapi.lib", 0x1460, 0xC
endlabel StopPAD2
    .space 0x4
