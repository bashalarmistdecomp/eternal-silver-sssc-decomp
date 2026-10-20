.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching setjmp, 0x3C

glabel setjmp
    .incbin "expected/code.bin", 0x842E8, 0x3C
endlabel setjmp
