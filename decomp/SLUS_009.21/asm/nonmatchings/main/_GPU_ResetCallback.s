.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _GPU_ResetCallback, 0x28

glabel _GPU_ResetCallback
    .incbin "expected/code.bin", 0x1141A0, 0x28
endlabel _GPU_ResetCallback
