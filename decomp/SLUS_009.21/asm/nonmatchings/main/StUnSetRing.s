.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching StUnSetRing, 0x80

glabel StUnSetRing
    .incbin "expected/code.bin", 0x118240, 0x80
endlabel StUnSetRing
