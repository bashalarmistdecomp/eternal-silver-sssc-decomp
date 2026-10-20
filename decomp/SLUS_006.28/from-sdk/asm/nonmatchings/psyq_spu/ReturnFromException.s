.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libapi ReturnFromException: PSY-Q 4.5, lib/libapi.lib, member A23, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching ReturnFromException, 0xC

glabel ReturnFromException
    .incbin "lib/libapi.lib", 0x163F, 0xC
endlabel ReturnFromException
    .space 0x4
