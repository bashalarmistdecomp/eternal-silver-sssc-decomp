.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libapi PAD_init2: PSY-Q 4.5, lib/libapi.lib, member A21, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching PAD_init2, 0xC

glabel PAD_init2
    .incbin "lib/libapi.lib", 0x14FB, 0xC
endlabel PAD_init2
    .space 0x4
