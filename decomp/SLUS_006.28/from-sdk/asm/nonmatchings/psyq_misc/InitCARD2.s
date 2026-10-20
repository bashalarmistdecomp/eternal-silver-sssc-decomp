.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libcard InitCARD2: PSY-Q 4.5, lib/libcard.lib, member A74, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching InitCARD2, 0xC

glabel InitCARD2
    .incbin "lib/libcard.lib", 0x2FB, 0xC
endlabel InitCARD2
    .space 0x4
