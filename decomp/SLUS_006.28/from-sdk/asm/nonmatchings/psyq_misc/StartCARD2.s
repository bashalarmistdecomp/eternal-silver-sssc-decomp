.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libcard StartCARD2: PSY-Q 4.5, lib/libcard.lib, member A75, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching StartCARD2, 0xC

glabel StartCARD2
    .incbin "lib/libcard.lib", 0x398, 0xC
endlabel StartCARD2
    .space 0x4
