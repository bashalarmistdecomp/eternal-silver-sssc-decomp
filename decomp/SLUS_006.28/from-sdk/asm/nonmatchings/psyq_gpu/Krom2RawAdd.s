.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libapi Krom2RawAdd: PSY-Q 4.5, lib/libapi.lib, member A81, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching Krom2RawAdd, 0xC

glabel Krom2RawAdd
    .incbin "lib/libapi.lib", 0x258E, 0xC
endlabel Krom2RawAdd
    .space 0x4
