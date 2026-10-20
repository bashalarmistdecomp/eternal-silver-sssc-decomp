.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libapi GPU_cw: PSY-Q 4.5, lib/libapi.lib, member C73, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching GPU_cw, 0xC

glabel GPU_cw
    .incbin "lib/libapi.lib", 0x409, 0xC
endlabel GPU_cw
    .space 0x4
