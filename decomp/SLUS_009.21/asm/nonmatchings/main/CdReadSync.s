.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching CdReadSync, 0x104

glabel CdReadSync
    .incbin "expected/code.bin", 0x117FFC, 0x104
endlabel CdReadSync
