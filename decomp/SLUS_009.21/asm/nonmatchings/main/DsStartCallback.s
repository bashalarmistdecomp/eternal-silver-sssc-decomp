.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching DsStartCallback, 0x14

glabel DsStartCallback
    .incbin "expected/code.bin", 0x119DA8, 0x14
endlabel DsStartCallback
