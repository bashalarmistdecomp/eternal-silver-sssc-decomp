.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching _card_write, 0xC

glabel _card_write
    .incbin "expected/code.bin", 0x929B8, 0xC
endlabel _card_write
    .incbin "expected/code.bin", 0x929C4, 0x4
