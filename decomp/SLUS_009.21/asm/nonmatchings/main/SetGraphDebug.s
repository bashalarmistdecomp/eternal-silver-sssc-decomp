.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching SetGraphDebug, 0x5C

glabel SetGraphDebug
    .incbin "expected/code.bin", 0x111434, 0x5C
endlabel SetGraphDebug
