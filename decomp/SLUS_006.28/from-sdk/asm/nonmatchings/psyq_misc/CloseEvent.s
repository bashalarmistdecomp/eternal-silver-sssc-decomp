.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libapi CloseEvent: PSY-Q 4.5, lib/libapi.lib, member A09, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching CloseEvent, 0xC

glabel CloseEvent
    .incbin "lib/libapi.lib", 0xE58, 0xC
endlabel CloseEvent
    .space 0x4
