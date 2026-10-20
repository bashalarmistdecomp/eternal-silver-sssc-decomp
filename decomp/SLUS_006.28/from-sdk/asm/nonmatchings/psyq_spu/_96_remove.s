.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libapi _96_remove: PSY-Q 4.5, lib/libapi.lib, member C114, .text + 0x8.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching _96_remove, 0xC

glabel _96_remove
    .incbin "lib/libapi.lib", 0x712, 0xC
endlabel _96_remove
    .space 0xC
