.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libapi lseek: PSY-Q 4.5, lib/libapi.lib, member A51, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching lseek, 0xC

glabel lseek
    .incbin "lib/libapi.lib", 0x1D62, 0xC
endlabel lseek
    .space 0x4
