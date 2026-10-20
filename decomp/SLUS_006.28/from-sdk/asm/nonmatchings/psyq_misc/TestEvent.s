.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libapi TestEvent: PSY-Q 4.5, lib/libapi.lib, member A11, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching TestEvent, 0xC

glabel TestEvent
    .incbin "lib/libapi.lib", 0xF8E, 0xC
endlabel TestEvent
    .space 0x4
