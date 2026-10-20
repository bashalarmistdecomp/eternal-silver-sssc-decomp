.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libapi OpenEvent: PSY-Q 4.5, lib/libapi.lib, member A08, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching OpenEvent, 0xC

glabel OpenEvent
    .incbin "lib/libapi.lib", 0xDBB, 0xC
endlabel OpenEvent
    .space 0x4
