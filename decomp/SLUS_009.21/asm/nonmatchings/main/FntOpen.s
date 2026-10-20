.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching FntOpen, 0x2B8

glabel FntOpen
    .incbin "expected/code.bin", 0x110360, 0x2B8
endlabel FntOpen
