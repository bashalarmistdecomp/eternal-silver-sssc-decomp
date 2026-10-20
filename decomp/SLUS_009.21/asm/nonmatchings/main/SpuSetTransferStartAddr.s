.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching SpuSetTransferStartAddr, 0x5C

glabel SpuSetTransferStartAddr
    .incbin "expected/code.bin", 0x11B6B0, 0x5C
endlabel SpuSetTransferStartAddr
    .incbin "expected/code.bin", 0x11B70C, 0x4
