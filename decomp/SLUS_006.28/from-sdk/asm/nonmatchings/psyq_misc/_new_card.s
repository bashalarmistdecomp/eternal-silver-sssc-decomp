.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libcard _new_card: PSY-Q 4.5, lib/libcard.lib, member A80, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching _new_card, 0xC

glabel _new_card
    .incbin "lib/libcard.lib", 0x60A, 0xC
endlabel _new_card
    .space 0x4
