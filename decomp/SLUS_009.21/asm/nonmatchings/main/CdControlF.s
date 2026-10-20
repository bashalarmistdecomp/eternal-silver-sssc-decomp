.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching CdControlF, 0x134

glabel CdControlF
    .incbin "expected/code.bin", 0x115358, 0x134
endlabel CdControlF
