.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching SetGraphQueue, 0xA4

glabel SetGraphQueue
    .incbin "expected/code.bin", 0x111490, 0xA4
endlabel SetGraphQueue
