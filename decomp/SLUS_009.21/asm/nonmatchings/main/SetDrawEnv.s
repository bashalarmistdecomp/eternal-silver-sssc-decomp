.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching SetDrawEnv, 0x1F8

glabel SetDrawEnv
    .incbin "expected/code.bin", 0x112488, 0x1F8
endlabel SetDrawEnv
