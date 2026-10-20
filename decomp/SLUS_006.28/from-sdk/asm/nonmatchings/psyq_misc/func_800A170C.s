.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libcard (no exported name): PSY-Q 4.5, lib/libcard.lib, member PATCH, .text + 0xB4.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching func_800A170C, 0x24

glabel func_800A170C
    .incbin "lib/libcard.lib", 0xAD6, 0x24
endlabel func_800A170C
    .space 0x4
