.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching DrawSyncCallback, 0x60

glabel DrawSyncCallback
    .incbin "expected/code.bin", 0x111544, 0x60
endlabel DrawSyncCallback
