.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching DsSyncCallback, 0x14

glabel DsSyncCallback
    .incbin "expected/code.bin", 0x119D80, 0x14
endlabel DsSyncCallback
