.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libapi WaitEvent: PSY-Q 4.5, lib/libapi.lib, member A10, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching WaitEvent, 0xC

glabel WaitEvent
    .incbin "lib/libapi.lib", 0xEF3, 0xC
endlabel WaitEvent
    .space 0x4
