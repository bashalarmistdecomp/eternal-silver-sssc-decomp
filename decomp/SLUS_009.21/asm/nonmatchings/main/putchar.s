.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching putchar, 0x128

glabel putchar
    .incbin "expected/code.bin", 0x120798, 0x128
endlabel putchar
