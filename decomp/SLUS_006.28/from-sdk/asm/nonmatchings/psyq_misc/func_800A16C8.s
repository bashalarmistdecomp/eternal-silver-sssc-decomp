.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libcard (no exported name): PSY-Q 4.5, lib/libcard.lib, member PATCH, .text + 0x70.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching func_800A16C8, 0x44

glabel func_800A16C8
    .incbin "lib/libcard.lib", 0xA92, 0x44
endlabel func_800A16C8
