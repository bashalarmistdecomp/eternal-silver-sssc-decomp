.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* Handwritten function */
/* libgte SetGeomOffset: PSY-Q 4.5, lib/libgte.lib, member REG12, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching SetGeomOffset, 0x18

glabel SetGeomOffset
    .incbin "lib/libgte.lib", 0x90C2, 0x18
endlabel SetGeomOffset
    .space 0x8
