.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching GetTimSize, 0xAC

glabel GetTimSize
    .incbin "expected/code.bin", 0x110D00, 0xAC
endlabel GetTimSize
