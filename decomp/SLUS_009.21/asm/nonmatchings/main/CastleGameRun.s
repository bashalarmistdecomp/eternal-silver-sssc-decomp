.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching CastleGameRun, 0x8870

glabel CastleGameRun
    .incbin "expected/code.bin", 0x10490C, 0x8870
endlabel CastleGameRun
