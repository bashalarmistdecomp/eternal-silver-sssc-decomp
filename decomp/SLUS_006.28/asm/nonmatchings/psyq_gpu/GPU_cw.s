.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching GPU_cw, 0xC

glabel GPU_cw
    .incbin "expected/code.bin", 0x7D7B8, 0xC
endlabel GPU_cw
    .incbin "expected/code.bin", 0x7D7C4, 0x4
