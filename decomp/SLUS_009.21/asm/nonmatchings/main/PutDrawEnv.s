.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching PutDrawEnv, 0xC0

glabel PutDrawEnv
    .incbin "expected/code.bin", 0x111CA0, 0xC0
endlabel PutDrawEnv
