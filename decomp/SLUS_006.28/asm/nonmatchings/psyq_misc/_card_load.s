.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _card_load, 0xC

glabel _card_load
    .incbin "expected/code.bin", 0x929A8, 0xC
endlabel _card_load
    .incbin "expected/code.bin", 0x929B4, 0x4
