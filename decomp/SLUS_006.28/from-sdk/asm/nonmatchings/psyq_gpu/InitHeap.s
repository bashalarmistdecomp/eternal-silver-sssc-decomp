.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libapi InitHeap: PSY-Q 4.5, lib/libapi.lib, member C57, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching InitHeap, 0xC

glabel InitHeap
    .incbin "lib/libapi.lib", 0x89, 0xC
endlabel InitHeap
    .space 0x4
