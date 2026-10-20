.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _padClrIntSio0, 0x90

glabel _padClrIntSio0
    .incbin "expected/code.bin", 0x11D5AC, 0x90
endlabel _padClrIntSio0
