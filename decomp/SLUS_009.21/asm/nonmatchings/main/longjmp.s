.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching longjmp, 0x3C

glabel longjmp
    .incbin "expected/code.bin", 0x12125C, 0x3C
endlabel longjmp
    .incbin "expected/code.bin", 0x121298, 0x8
