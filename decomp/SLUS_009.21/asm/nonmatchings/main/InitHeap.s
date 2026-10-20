.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching InitHeap

dlabel InitHeap
    .incbin "expected/code.bin", 0x10FD50, 0x18
enddlabel InitHeap
