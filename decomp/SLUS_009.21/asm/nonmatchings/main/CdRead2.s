.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching CdRead2, 0x84

glabel CdRead2
    .incbin "expected/code.bin", 0x118130, 0x84
endlabel CdRead2
