.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _new_card, 0xC

glabel _new_card
    .incbin "expected/code.bin", 0x929C8, 0xC
endlabel _new_card
    .incbin "expected/code.bin", 0x929D4, 0x4
