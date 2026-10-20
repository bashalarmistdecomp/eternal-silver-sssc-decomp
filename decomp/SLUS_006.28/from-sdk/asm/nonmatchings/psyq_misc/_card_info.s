.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libcard _card_info: PSY-Q 4.5, lib/libcard.lib, member C171, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching _card_info, 0xC

glabel _card_info
    .incbin "lib/libcard.lib", 0x126, 0xC
endlabel _card_info
    .space 0x4
