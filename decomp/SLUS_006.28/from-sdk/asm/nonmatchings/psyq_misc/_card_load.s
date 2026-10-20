.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libcard _card_load: PSY-Q 4.5, lib/libcard.lib, member C172, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching _card_load, 0xC

glabel _card_load
    .incbin "lib/libcard.lib", 0x1C3, 0xC
endlabel _card_load
    .space 0x4
