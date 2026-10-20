.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching PutDispEnv, 0x4F8

glabel PutDispEnv
    .incbin "expected/code.bin", 0x111E6C, 0x4F8
endlabel PutDispEnv
