.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libcard _card_write: PSY-Q 4.5, lib/libcard.lib, member A78, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching _card_write, 0xC

glabel _card_write
    .incbin "lib/libcard.lib", 0x4D2, 0xC
endlabel _card_write
    .space 0x4
