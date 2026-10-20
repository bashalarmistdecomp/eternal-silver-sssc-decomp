.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching longjmp, 0x3C

glabel longjmp
    .incbin "expected/code.bin", 0x84324, 0x3C
endlabel longjmp
    .incbin "expected/code.bin", 0x84360, 0x8
