.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching MDEC_out_sync, 0x94

glabel MDEC_out_sync
    .incbin "expected/code.bin", 0x1197E4, 0x94
endlabel MDEC_out_sync
