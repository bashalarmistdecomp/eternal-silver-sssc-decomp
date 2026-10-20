.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching FntPrint, 0x3C0

glabel FntPrint
    .incbin "expected/code.bin", 0x110934, 0x3C0
endlabel FntPrint
    .incbin "expected/code.bin", 0x110CF4, 0xC
