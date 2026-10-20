.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching GPU_cw, 0xBC

glabel GPU_cw
    .incbin "expected/code.bin", 0x114220, 0xBC
endlabel GPU_cw
