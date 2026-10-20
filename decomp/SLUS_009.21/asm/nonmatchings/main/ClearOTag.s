.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */

nonmatching ClearOTag, 0xC8

glabel ClearOTag
    .incbin "expected/code.bin", 0x111A60, 0xC8
endlabel ClearOTag
