.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* Handwritten function */
/* libgte SetColorMatrix: PSY-Q 4.5, lib/libgte.lib, member MTX_11, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching SetColorMatrix, 0x30

glabel SetColorMatrix
    .incbin "lib/libgte.lib", 0x840D, 0x30
endlabel SetColorMatrix
