.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libcard _card_read: PSY-Q 4.5, lib/libcard.lib, member A79, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching _card_read, 0xC

glabel _card_read
    .incbin "lib/libcard.lib", 0x56F, 0xC
endlabel _card_read
    .space 0x4
