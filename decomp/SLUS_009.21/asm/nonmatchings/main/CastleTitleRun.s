.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching CastleTitleRun, 0xA34

glabel CastleTitleRun
    .incbin "expected/code.bin", 0xFE6F4, 0xA34
endlabel CastleTitleRun
