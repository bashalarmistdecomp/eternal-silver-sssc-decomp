.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _putchar, 0xFC

glabel _putchar
    .incbin "expected/code.bin", 0x120660, 0xFC
endlabel _putchar
