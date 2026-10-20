.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* Handwritten function */
/* libgte (no exported name): PSY-Q 4.5, lib/libgte.lib, member PATCHGTE, .text + 0xAC.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching func_80090044, 0x30

glabel func_80090044
    .incbin "lib/libgte.lib", 0x4691A, 0x18
  alabel D_8009005C
    .incbin "lib/libgte.lib", 0x46932, 0x18
endlabel func_80090044
  alabel D_80090074
    .space 0x4
