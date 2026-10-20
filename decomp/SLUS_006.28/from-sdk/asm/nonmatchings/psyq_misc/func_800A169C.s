.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libcard (no exported name): PSY-Q 4.5, lib/libcard.lib, member PATCH, .text + 0x44.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching func_800A169C, 0x2C

glabel func_800A169C
    .incbin "lib/libcard.lib", 0xA66, 0x2C
endlabel func_800A169C
