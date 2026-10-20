.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* Handwritten function */
/* libgte SetFarColor: PSY-Q 4.5, lib/libgte.lib, member REG11, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching SetFarColor, 0x20

glabel SetFarColor
    .incbin "lib/libgte.lib", 0x900F, 0x20
endlabel SetFarColor
