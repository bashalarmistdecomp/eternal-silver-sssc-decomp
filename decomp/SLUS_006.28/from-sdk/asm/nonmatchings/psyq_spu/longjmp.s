.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

/* libc2 longjmp: PSY-Q 4.5, lib/libc2.lib, member SETJMP, .text + 0x3C.
   These bytes are read from Sony's own library file, which `recipe/toolchain.sh sdk`
   fetches, not from the disc (BUILD.md). */
nonmatching longjmp, 0x3C

glabel longjmp
    .incbin "lib/libc2.lib", 0x579B, 0x3C
endlabel longjmp
    .space 0x8
