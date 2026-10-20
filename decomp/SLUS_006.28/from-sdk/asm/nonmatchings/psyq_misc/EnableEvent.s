.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libapi EnableEvent: PSY-Q 4.5, lib/libapi.lib, member A12, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching EnableEvent, 0xC

glabel EnableEvent
    .incbin "lib/libapi.lib", 0x102D, 0xC
endlabel EnableEvent
    .space 0x4
