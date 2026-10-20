.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libapi (no exported name): PSY-Q 4.5, lib/libapi.lib, member C114, .text + 0x0.
   These bytes are read from Sony's own file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching func_80095298

dlabel func_80095298
    .incbin "lib/libapi.lib", 0x70A, 0x8
enddlabel func_80095298
