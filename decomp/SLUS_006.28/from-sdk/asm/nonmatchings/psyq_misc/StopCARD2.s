.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libcard StopCARD2: PSY-Q 4.5, lib/libcard.lib, member A76, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching StopCARD2, 0xC

glabel StopCARD2
    .incbin "lib/libcard.lib", 0x433, 0xC
endlabel StopCARD2
    .space 0x4
