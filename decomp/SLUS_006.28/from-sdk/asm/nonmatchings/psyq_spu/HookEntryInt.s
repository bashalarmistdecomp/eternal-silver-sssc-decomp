.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libapi HookEntryInt: PSY-Q 4.5, lib/libapi.lib, member A25, .text + 0x0.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching HookEntryInt, 0xC

glabel HookEntryInt
    .incbin "lib/libapi.lib", 0x1783, 0xC
endlabel HookEntryInt
    .space 0x4
