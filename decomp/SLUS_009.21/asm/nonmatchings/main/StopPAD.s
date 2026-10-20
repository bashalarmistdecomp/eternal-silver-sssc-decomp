.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching StopPAD, 0x38

glabel StopPAD
    .incbin "expected/code.bin", 0x10FF6C, 0x38
endlabel StopPAD
