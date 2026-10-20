.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching LoadImage2, 0xEC

glabel LoadImage2
    .incbin "expected/code.bin", 0x113D88, 0xEC
endlabel LoadImage2
