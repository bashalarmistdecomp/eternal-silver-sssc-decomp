.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching MDEC_in_sync, 0x94

glabel MDEC_in_sync
    .incbin "expected/code.bin", 0x119750, 0x94
endlabel MDEC_in_sync
