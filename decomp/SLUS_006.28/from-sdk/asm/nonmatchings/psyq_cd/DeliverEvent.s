.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libapi DeliverEvent: PSY-Q 4.5, lib/libapi.lib, member A07, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching DeliverEvent, 0xC

glabel DeliverEvent
    .incbin "lib/libapi.lib", 0xD20, 0xC
endlabel DeliverEvent
    .space 0x4
