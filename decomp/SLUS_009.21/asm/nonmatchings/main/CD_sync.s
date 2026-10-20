.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching CD_sync, 0x280

glabel CD_sync
    .incbin "expected/code.bin", 0x115BE4, 0x280
endlabel CD_sync
