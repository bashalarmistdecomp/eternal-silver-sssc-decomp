.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching SpuIsTransferCompleted, 0xA4

glabel SpuIsTransferCompleted
    .incbin "expected/code.bin", 0x11B740, 0xA4
endlabel SpuIsTransferCompleted
    .incbin "expected/code.bin", 0x11B7E4, 0xC
