.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching DsReadyCallback, 0x14

glabel DsReadyCallback
    .incbin "expected/code.bin", 0x119D94, 0x14
endlabel DsReadyCallback
