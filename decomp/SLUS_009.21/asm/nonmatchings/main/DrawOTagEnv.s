.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching DrawOTagEnv, 0xD8

glabel DrawOTagEnv
    .incbin "expected/code.bin", 0x111D60, 0xD8
endlabel DrawOTagEnv
