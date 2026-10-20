.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching ResetGraph, 0x174

glabel ResetGraph
    .incbin "expected/code.bin", 0x1112C0, 0x174
endlabel ResetGraph
